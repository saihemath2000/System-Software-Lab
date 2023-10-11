#ifndef STUDENT_FUNCTIONS
#define STUDENT_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include "server-constants.h"

struct Student loggedInStudent;
struct Enrollment enroll;
int semid;

int student_operation_handler(int connFD);
int Logout(int connFD);
int Change_password(int connFD);
int view_all_courses(int connFD);
int enroll_course(int connFD);
int view_enrolled_courses(int connFD);
int drop_course(int connFD);
int student_operation_handler(int connFD){

    if(login_handler(3,connFD,NULL,&loggedInStudent)){
        
        key_t semKey = ftok(STUDENT_FILE,loggedInStudent.id); // Generate a key based on the account number hence, different customers will have different semaphores
        union semun
        {
          int val; // Value of the semaphore
        } semSet;

        int semctlStatus;
        semid = semget(semKey, 1, 0); // Get the semaphore if it exists
        if (semid == -1)
        {
            semid = semget(semKey, 1, IPC_CREAT | 0700); // Create a new semaphore
            if (semid == -1)
            {
                perror("Error while creating semaphore!");
                _exit(1);
            }

            semSet.val = 1; // Set a binary semaphore
            semctlStatus = semctl(semid, 0, SETVAL, semSet);
            if (semctlStatus == -1)
            {
                perror("Error while initializing a binary sempahore!");
                _exit(1);
            }
        }
        ssize_t writeBytes, readBytes;            
        char readBuffer[1000], writeBuffer[1000];
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, LOGIN_SUCCESS);
        while(1){
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, STUDENT_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                // perror("Error while writing STUDENT_MENU to client!");
                return 0;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for STUDENT_MENU");
                return 0;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                view_all_courses(connFD);
                break;
            case 2:
                enroll_course(connFD);
                 break;
            case 3: 
                drop_course(connFD);
                break;
            case 4:
                view_enrolled_courses(connFD);
                break;
            case 5:
                Change_password(connFD);
                break;
            case 6:
                Logout(connFD);
                break;    
            default:
                return 0;
            }
        }
    }
    else
    {
        // STUDENT LOGIN FAILED
        return 0;
    }
    return 1;
}

int drop_course(int connFD){
  ssize_t readBytes, writeBytes;             
  char readBuffer[1000], writeBuffer[10000];
  int enrollFileDescriptor, courseFileDescriptor;
  bool flag;
  struct Enrollment enroll;
  struct Course course;
  int enrollID,n;
  struct flock lock = {F_WRLCK, SEEK_SET, 0, sizeof(struct Enrollment), getpid()};
  if(strcmp(loggedInStudent.access,"blocked")==0){
    write(connFD,"You are blocked by admin ^",26);
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 0;
  }
  writeBytes = write(connFD,"Enter the course-id you want to drop:",37);
  readBytes = read(connFD,readBuffer,sizeof(readBuffer));
  if(readBytes==-1){
    perror("unable to get unenroll courseid response from client");
    return 0;
  }
  
  enrollFileDescriptor = open(ENROLL_FILE,O_RDONLY);
  while((n = read(enrollFileDescriptor, &enroll, sizeof(struct Enrollment))) > 0) {
    if((strcmp(enroll.status,"enrolled")==0) && (strcmp(enroll.studentid,loggedInStudent.loginid)==0) && (strcmp(enroll.courseid,readBuffer)==0)){              
        enrollID= enroll.id;
        flag=true;
        break;  
    }
  }
  close(enrollFileDescriptor);
  
  if(flag==false){
    // sprintf(writeBuffer,"%d",enrollID);
    // write(connFD,writeBuffer,sizeof(writeBuffer));
    write(connFD,"Invalid course id1 ^",20);
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 0;
  } 
  
  enrollFileDescriptor = open(ENROLL_FILE,O_RDONLY);
  int offset = lseek(enrollFileDescriptor,-sizeof(struct Enrollment),SEEK_END);
  readBytes = read(enrollFileDescriptor, &enroll, sizeof(struct Enrollment));
  if(readBytes == -1){
    perror("Error reading enrollment record from file!");
    return false;
  }
  if(enrollID>enroll.id){
    write(connFD,"Invalid course id2 ^",20);
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 0;
   }
   close(enrollFileDescriptor);

  enrollFileDescriptor = open(ENROLL_FILE,O_RDONLY);
  offset = lseek(enrollFileDescriptor,(enrollID-1)*sizeof(struct Enrollment),SEEK_SET);
  if (offset == -1){
    perror("Error while seeking to required course record!");
    return 0;
  }
  lock.l_type = F_RDLCK;
  lock.l_start = offset;
  int lockingStatus = fcntl(enrollFileDescriptor, F_SETLKW, &lock);
  if (lockingStatus == -1){
      perror("Error while obtaining read lock on course record!");
      return 0;
  }
  readBytes = read(enrollFileDescriptor, &enroll, sizeof(struct Enrollment));
  if(readBytes == -1){
    perror("Error while reading enrollment record from the file!");
    return 0;
  }
  if(strcmp(enroll.status,"unenrolled")==0){
      write(connFD,"Not enrolled ^",14);
      readBytes = read(connFD,readBuffer,sizeof(readBuffer));
      return 0;
  }
  lock.l_type = F_UNLCK;
  lockingStatus = fcntl(courseFileDescriptor, F_SETLK, &lock);   
  close(enrollFileDescriptor);

  strcpy(enroll.status,"unenrolled");
  
  enrollFileDescriptor = open(ENROLL_FILE,O_WRONLY);
  if (enrollFileDescriptor == -1){
        perror("Error while opening enrollment file");
        return 0;
  }

  offset = lseek(enrollFileDescriptor, (enrollID-1) * sizeof(struct Enrollment), SEEK_SET);
  if(offset == -1){
    perror("Error while seeking to required enrollment record!");
    return 0;
  }
  lock.l_type = F_WRLCK;
  lock.l_start = offset;
  lockingStatus = fcntl(enrollFileDescriptor, F_SETLKW, &lock);
  if(lockingStatus == -1){
    perror("Error while obtaining write lock on enrollment record!");
    return 0;
  }
  writeBytes = write(enrollFileDescriptor, &enroll, sizeof(struct Enrollment));
  if (writeBytes == -1){
    perror("Error while writing update enrollment info into file");
  }

  lock.l_type = F_UNLCK;
  fcntl(enrollFileDescriptor, F_SETLKW, &lock);

  close(enrollFileDescriptor);
  
  int courseID;
  courseFileDescriptor = open(COURSE_FILE,O_RDONLY);
  while((n = read(courseFileDescriptor, &course, sizeof(struct Course))) > 0) {
    if((strcmp(course.status,"notactive")!=0) && (strcmp(course.courseid,readBuffer)==0)){              
        courseID= course.id;
        break;  
    }
  }
  close(courseFileDescriptor);
  
  courseFileDescriptor = open(COURSE_FILE,O_RDONLY);
  offset = lseek(courseFileDescriptor,(courseID-1)*sizeof(struct Course),SEEK_SET);
  if (offset == -1){
    perror("Error while seeking to required course record!");
    return 0;
  }
  lock.l_type = F_RDLCK;
  lock.l_start = offset;
  lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
  if (lockingStatus == -1){
      perror("Error while obtaining read lock on course record!");
      return 0;
  }
  readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
  if(readBytes == -1){
    perror("Error while reading course record from the file!");
    return 0;
  }
  lock.l_type = F_UNLCK;
  lockingStatus = fcntl(courseFileDescriptor, F_SETLK, &lock);   
  close(enrollFileDescriptor);

  course.no_of_available_seats=course.no_of_available_seats+1;
  
  courseFileDescriptor = open(COURSE_FILE,O_WRONLY);
  if (courseFileDescriptor == -1){
        perror("Error while opening course file");
        return 0;
  }
  offset = lseek(courseFileDescriptor, (courseID-1) * sizeof(struct Course), SEEK_SET);
  if(offset == -1){
    perror("Error while seeking to required course record!");
    return 0;
  }
  lock.l_type = F_WRLCK;
  lock.l_start = offset;
  lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
  if(lockingStatus == -1){
    perror("Error while obtaining write lock on course record!");
    return 0;
  }
  writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
  if (writeBytes == -1){
    perror("Error while writing update course info into file");
  }

  lock.l_type = F_UNLCK;
  fcntl(courseFileDescriptor, F_SETLKW, &lock);

  close(courseFileDescriptor);
  writeBytes = write(connFD, ADD_UNENROLL_SUCCESS,strlen(ADD_UNENROLL_SUCCESS));
  if(writeBytes == -1){
      perror("Error writing ADD_UNENROLL_SUCCESS message to client!");
      return false;
  }
  readBytes = read(connFD,readBuffer,sizeof(readBuffer));
  return 0;
}

int view_enrolled_courses(int connFD){
    ssize_t readBytes, writeBytes;             
    char readBuffer[1000], writeBuffer[10000]; 
    char tempBuffer[1000];
    char temp1[15][10];
    int i,n;
    int enrollFileDescriptor;
    int courseFileDescriptor;
    struct Enrollment enroll;
    struct Course course;
    bool flag;
    
    if(strcmp(loggedInStudent.access,"blocked")==0){
       write(connFD,"You are blocked by admin ^",26);
       readBytes = read(connFD,readBuffer,sizeof(readBuffer));
       return 0;
    }

    enrollFileDescriptor = open(ENROLL_FILE,O_RDONLY);
    while((n = read(enrollFileDescriptor, &enroll, sizeof(struct Enrollment))) > 0) {
        if((strcmp(enroll.status,"enrolled")==0) && (strcmp(enroll.studentid,loggedInStudent.loginid)==0)){              
           flag=true;
           strcpy(temp1[i],enroll.courseid);
           i++;   
        }
    }
    if(flag==false){
       write(connFD,"No enrolled courses ^",21);
       readBytes = read(connFD,readBuffer,sizeof(readBuffer));  
    }
    bool magic[20]={false};
    for(int i=0;i<15;i++){
        courseFileDescriptor = open(COURSE_FILE,O_RDONLY);
        while((n = read(courseFileDescriptor, &course, sizeof(struct Course))) > 0) {
            if(magic[course.id]==false && (strcmp(course.status,"notactive")!=0) && (strcmp(course.courseid,temp1[i])==0)){                 
                bzero(writeBuffer,sizeof(writeBuffer));
                magic[course.id]=true;   
                sprintf(writeBuffer, " ^ ********* Course Details *********  \n\tName: %s\n\tDepartment : %s\n\tNo of Seats: %d\n\tCredits : %d\n\tNo of available seats: %d\n\tCourse-id: %s\n", course.name, course.department,course.no_of_seats,course.credits,course.no_of_available_seats,course.courseid);
                writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
                if(writeBytes == -1){
                    perror("Error writing course info to client!");
                    return 0;
                }
                readBytes = read(connFD,readBuffer,sizeof(readBuffer));
                break;
            } 
        }
        close(courseFileDescriptor);
    }
    return 1;   
}

int enroll_course(int connFD){
    ssize_t readBytes, writeBytes;             
    char readBuffer[1000], writeBuffer[10000]; 
    char tempBuffer[1000];

    struct Course fetchcourse;
    int courseFileDescriptor;
    struct flock lock = {F_WRLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};
    
    if(strcmp(loggedInStudent.access,"blocked")==0){
       write(connFD,"You are blocked by admin ^",26);
       readBytes = read(connFD,readBuffer,sizeof(readBuffer));
       return 0;
    }

    courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Course File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "course file id doesn't exists");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes ==-1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        return 0;
    }
    int n;
    int i=1;
    char temp1[15][10];
    strcpy(readBuffer,"\n........Available Courses.......\n");
    while((n = read(courseFileDescriptor, &fetchcourse, sizeof(struct Course))) > 0) {
        // strcpy(readBuffer,fetchcourse.status);
        char temp[200];
        if(strcmp(fetchcourse.status,"notactive")==0)
           continue;
        strcpy(temp1[i],fetchcourse.courseid);
        i++;   
        strcpy(temp,fetchcourse.courseid);
        strcat(temp,"\t--\t");
        strcat(temp,fetchcourse.name);
        strcat(temp,"\n");   
        strcat(readBuffer,temp);   
    }
    close(courseFileDescriptor);
    strcat(readBuffer,"\nEnter course id you want to enroll:");
    writeBytes = write(connFD, readBuffer, strlen(readBuffer));
    if(writeBytes == -1){
        perror("Error writing course info to client!");
        return 0;
    }
    bzero(readBuffer,sizeof(readBuffer));
    readBytes= read(connFD,readBuffer,sizeof(readBuffer));
    if(readBytes==-1){
        perror("unable to read choice from client");
        return 0;
    }
    int courseID;
    for(int i=0;i<15;i++){
        if(strcmp(readBuffer,temp1[i])==0){
            char *ftPosition = strstr(readBuffer, "C-");
            char *numberStart = NULL;
            if(ftPosition!=NULL) {
               numberStart = ftPosition + strlen("C-");
               courseID = atoi(numberStart);
            }
            struct Course course;
            courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
            int offset = lseek(courseFileDescriptor, (courseID-1) * sizeof(struct Course), SEEK_SET);
            if (offset == -1){
               perror("Error while seeking to required course record!");
               return 0;
            }
            lock.l_type = F_RDLCK;
            lock.l_start = offset;
            int lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1){
              perror("Error while obtaining read lock on course record!");
              return 0;
            }

            readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
            if(readBytes == -1){
                perror("Error while reading course record from the file!");
                return 0;
            }
            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(courseFileDescriptor, F_SETLK, &lock);
            
            if(course.no_of_available_seats==0){
               write(connFD,"No available seats to enroll ^",30);
               close(courseFileDescriptor);
               readBytes = read(connFD,readBuffer,sizeof(readBuffer));
               return 0;
            }
            else{
                int n,enrollID;
                bool flag=false;
                int enrollfd = open(ENROLL_FILE,O_RDONLY,0700);
                struct Enrollment enroll; 
                while((n = read(enrollfd,&enroll, sizeof(struct Enrollment)))> 0) {
                   if((strcmp(readBuffer,enroll.courseid)==0) && (strcmp(loggedInStudent.loginid,enroll.studentid)==0) && (strcmp(enroll.status,"enrolled")==0)){
                       writeBytes = write(connFD, "Already enrolled ^",18);
                       readBytes = read(connFD,readBuffer,sizeof(readBuffer));
                       return 0;                
                   }
                   else if((strcmp(readBuffer,enroll.courseid)==0) && (strcmp(loggedInStudent.loginid,enroll.studentid)==0) && (strcmp(enroll.status,"unenrolled")==0)){
                      flag=true;
                      enrollID= enroll.id;
                      break;                           
                   }       
                } 
                if(flag==true){
                    close(enrollfd);
                    enrollfd = open(ENROLL_FILE,O_RDONLY);
                    int offset = lseek(enrollfd,(enrollID-1)*sizeof(struct Enrollment),SEEK_SET);
                    if(offset == -1){
                        perror("Error while seeking to required course record!");
                        return 0;
                    }
                    lock.l_type = F_RDLCK;
                    lock.l_start = offset;
                    int lockingStatus = fcntl(enrollfd, F_SETLKW, &lock);
                    if(lockingStatus == -1){
                        perror("Error while obtaining read lock on enrollment record!");
                        return 0;
                    }
                    readBytes = read(enrollfd, &enroll, sizeof(struct Enrollment));
                    if(readBytes == -1){
                        perror("Error while reading enrollment record from the file!");
                        return 0;
                    }
                    lock.l_type = F_UNLCK;
                    lockingStatus = fcntl(enrollfd, F_SETLK, &lock);   
                    close(enrollfd);

                    strcpy(enroll.status,"enrolled");
                    enroll.enroll_time= time(NULL);
                    enrollfd = open(ENROLL_FILE,O_WRONLY);
                    if (enrollfd == -1){
                        perror("Error while opening enrollment file");
                        return 0;
                    }
                    offset = lseek(enrollfd, (enrollID-1) * sizeof(struct Enrollment), SEEK_SET);
                    if(offset == -1){
                      perror("Error while seeking to required enrollment record!");
                      return 0;
                    }
                    lock.l_type = F_WRLCK;
                    lock.l_start = offset;
                    lockingStatus = fcntl(enrollfd, F_SETLKW, &lock);
                    if(lockingStatus == -1){
                        perror("Error while obtaining write lock on enrollment record!");
                        return 0;
                    }
                    writeBytes = write(enrollfd, &enroll, sizeof(struct Enrollment));
                    if (writeBytes == -1){
                        perror("Error while writing update enrollment info into file");
                    }

                     lock.l_type = F_UNLCK;
                     fcntl(enrollfd, F_SETLKW, &lock);
                     close(enrollfd);
                } 
                else{
                    if(enrollfd== -1){
                     // Enrollment file was never created
                     enroll.id = 1;
                    }
                    else{
                        int offset = lseek(enrollfd,-sizeof(struct Enrollment),SEEK_END);
                        if(offset == -1){
                            perror("Error seeking to last enrollment record!");
                            return 0;
                        }
                        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Enrollment), getpid()};
                        int lockingStatus = fcntl(enrollfd, F_SETLKW, &lock);
                        if(lockingStatus == -1){
                            perror("Error obtaining read lock on enroll record!");
                            return 0;
                        }
                        struct Enrollment previousenroll;
                        readBytes = read(enrollfd,&previousenroll, sizeof(struct Enrollment));
                        if(readBytes == -1){
                            perror("Error while reading enrollment record from file!");
                            return 0;
                        }
                        lock.l_type = F_UNLCK;
                        fcntl(enrollfd, F_SETLK, &lock);
                        close(enrollfd);
 
                        enroll.id = previousenroll.id + 1;
                    }
                    strcpy(enroll.courseid,readBuffer);
                    strcpy(enroll.studentid,loggedInStudent.loginid);
                    strcpy(enroll.status,"enrolled");
                    enroll.enroll_time= time(NULL);
                    enrollfd = open(ENROLL_FILE,O_CREAT|O_APPEND|O_WRONLY,S_IRWXU);
                    if(enrollfd == -1){
                        perror("Error while opening enroll file!");
                        return 0;
                    }
                    writeBytes = write(enrollfd, &enroll,sizeof(struct Enrollment));
                    if(writeBytes == -1){
                        perror("Error while writing enroll record to file!");
                        return 0;
                    }
                } 
            }
            close(courseFileDescriptor);
            course.no_of_available_seats=course.no_of_available_seats-1;                 
            
            courseFileDescriptor = open(COURSE_FILE, O_WRONLY);
            if(courseFileDescriptor == -1){
                perror("Error while opening course file");
                return 0;
            }
            offset = lseek(courseFileDescriptor, (courseID-1) * sizeof(struct Course), SEEK_SET);
            if (offset == -1){
               perror("Error while seeking to required course record!");
               return false;
            }

            lock.l_type = F_WRLCK;
            lock.l_start = offset;
            lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1){
                perror("Error while obtaining write lock on course record!");
                return false;
            }
            writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
            if (writeBytes == -1){
                 perror("Error while writing update course info into file");
            }
            lock.l_type = F_UNLCK;
            fcntl(courseFileDescriptor, F_SETLKW, &lock);
            close(courseFileDescriptor);

            writeBytes = write(connFD, ADD_ENROLL_SUCCESS,strlen(ADD_ENROLL_SUCCESS));
            if(writeBytes == -1){
                perror("Error writing ADD_ENROLL_SUCCESS message to client!");
                return false;
            }
            readBytes = read(connFD,readBuffer,sizeof(readBuffer));
            return 0;
        }
    }
    writeBytes= write(connFD,"Invalid course ^",16);
    if(writeBytes==-1){
        perror("unable to write invalid course error message to client");
        return 0;
    }
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 1;
}
int view_all_courses(int connFD){
    
    ssize_t readBytes, writeBytes;             
    char readBuffer[1000], writeBuffer[10000]; 
    char tempBuffer[1000];

    struct Course fetchcourse;
    int courseFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};

    if(strcmp(loggedInStudent.access,"blocked")==0){
       write(connFD,"You are blocked by admin ^",26);
       readBytes = read(connFD,readBuffer,sizeof(readBuffer));
       return 0;
    }

    courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Course File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "course file id doesn't exists");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes ==-1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        return 0;
    }
    int n;
    while((n = read(courseFileDescriptor, &fetchcourse, sizeof(struct Course))) > 0) {
        strcpy(readBuffer,fetchcourse.status);
        if(strcmp(readBuffer,"notactive")==0)
           continue;
        bzero(writeBuffer,sizeof(writeBuffer));   
        sprintf(writeBuffer, " ^ ********* Course Details *********  \n\tName: %s\n\tDepartment : %s\n\tNo of Seats: %d\n\tCredits : %d\n\tNo of available seats: %d\n\tCourse-id: %s\n", fetchcourse.name, fetchcourse.department,fetchcourse.no_of_seats,fetchcourse.credits,fetchcourse.no_of_available_seats,fetchcourse.courseid);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if(writeBytes == -1){
          perror("Error writing course info to client!");
          return 0;
        }
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    }
    
    bzero(writeBuffer,sizeof(writeBuffer));
    strcat(writeBuffer, "\n\n You'll now be redirected to the Student menu ^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if(writeBytes == -1){
        perror("Error writing course info to client!");
        return 0;
    }
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 1;   
}

int Logout(int connFD){
    ssize_t readBytes, writeBytes;             
    char readBuffer[1000], writeBuffer[10000];
    char tempBuffer[1000];
    write(connFD,LOG_OUT,strlen(LOG_OUT));
    close(connFD);
    return 0;
}

int Change_password(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000], hashedPassword[1000];

    char newPassword[1000];

    // Lock the critical section
    struct sembuf semOp = {0, -1, SEM_UNDO};
    int semopStatus = semop(semid, &semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return 0;
    }

    writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
    if (writeBytes == -1)
    {
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading old password response from client");
        unlock_critical_section(&semOp);
        return 0;
    }
     
    if (strcmp(crypt(readBuffer, SALT_BAE), loggedInStudent.password) == 0)
    {
        // Password matches with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            unlock_critical_section(&semOp);
            return 0;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password response from client");
            unlock_critical_section(&semOp);
            return 0;
        }

        strcpy(newPassword, crypt(readBuffer, SALT_BAE));

        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
            unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password reenter response from client");
            unlock_critical_section(&semOp);
            return 0;
        }

        if (strcmp(crypt(readBuffer, SALT_BAE), newPassword) == 0)
        {
            // New & reentered passwords match

            strcpy(loggedInStudent.password, newPassword);

            int studentFileDescriptor = open(STUDENT_FILE, O_WRONLY);
            if (studentFileDescriptor == -1)
            {
                perror("Error opening student file!");
                unlock_critical_section(&semOp);
                return 0;
            }

            off_t offset = lseek(studentFileDescriptor, (loggedInStudent.id-1) * sizeof(struct Student), SEEK_SET);
            if (offset == -1)
            {
                perror("Error seeking to the student record!");
                unlock_critical_section(&semOp);
                return 0;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};
            int lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining write lock on student record!");
                unlock_critical_section(&semOp);
                return 0;
            }

            writeBytes = write(studentFileDescriptor, &loggedInStudent, sizeof(struct Student));
            if (writeBytes == -1)
            {
                perror("Error storing updated student password into student record!");
                unlock_critical_section(&semOp);
                return 0;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(studentFileDescriptor, F_SETLK, &lock);

            close(studentFileDescriptor);

            writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            unlock_critical_section(&semOp);

            return 1;
        }
        else
        {
            // New & reentered passwords don't match
            writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
            readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        }
    }
    else
    {
        // Password doesn't match with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    }

    unlock_critical_section(&semOp);
    return 0;  
}


#endif