#ifndef FACULTY_FUNCTIONS
#define FACULTY_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>
#include<time.h>
#include "server-constants.h"

struct Faculty loggedInFaculty;
struct middleware{
    int myid;
    time_t mytime;
};
struct middleware myDict[100];


int semid;
bool lock_critical_section(struct sembuf *semOp);
bool unlock_critical_section(struct sembuf *sem_op);
int faculty_operation_handler(int connFD);
int add_course(int connFD);
int view_offering_course(int connFD);
int remove_course(int connFD);
int modify_course(int connFD);
int change_password(int connFD);
int logout(int connFD);

int faculty_operation_handler(int connFD){
    if(login_handler(2,connFD,&loggedInFaculty,NULL)){
        key_t semKey = ftok(FACULTY_FILE,loggedInFaculty.id); // Generate a key based on the account number hence, different customers will have different semaphores
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
            strcat(writeBuffer, FACULTY_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing FACULTY_MENU to client!");
                return 0;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for FACULTY_MENU");
                return 0;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                view_offering_course(connFD);
                break;
            case 2:
                add_course(connFD);
                 break;
            case 3: 
                remove_course(connFD);
                break;
            case 4:
                modify_course(connFD);
                break;
            case 5:
                change_password(connFD);
                break;
            case 6:
                logout(connFD);
                break;    
            default:
                return 0;
            }
        }
    }
    else
    {
        // FACULTY LOGIN FAILED
        return 0;
    }
    return 1;
}

int remove_course(int connFD){
    
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;
    int courseID;
    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, DEL_COURSE_ID, strlen(DEL_COURSE_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing DEL_COURSE_ID message to client!");
        return 0;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading course ID from client!");
        return 0;
    }
    char *ftPosition = strstr(readBuffer, "C-");
    char *numberStart = NULL;
    if(ftPosition!=NULL) {
        // Move the pointer to the character right after "C-"
        numberStart = ftPosition + strlen("C-");
        // Convert the numeric part to an integer
        courseID = atoi(numberStart);
    }
    else{
        write(connFD,"wrong courseid",15);
        return 0;
    }

    int courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Course File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer,"course file id doesn't exists");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing course id doesnt exists message to client!");
            return 0;
        }
        return 0;
    }
    
    offset = lseek(courseFileDescriptor, (courseID-1)* sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // course record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer,"Course id doesn't exists");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing course id doesnt exists message to client!");
            return 0;
        }
        return 0;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return 0;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
    struct flock lock1 = {F_RDLCK,SEEK_SET,offset,sizeof(struct Enrollment),getpid()}; 
    // Lock the record to be read
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on course record!");
        return 0;
    }

    readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error while reading course record from the file!");
        return 0;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    close(courseFileDescriptor);
    if(strcmp(loggedInFaculty.loginid,course.facultyloginid)!=0){
        write(connFD,"Not your course to remove ^",27);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    else if(strcmp(course.status,"notactive")==0){
        write(connFD,"Course not found to remove ^",29);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }

    courseFileDescriptor = open(COURSE_FILE, O_WRONLY);
    if (courseFileDescriptor == -1)
    {
        perror("Error while opening course file");
        return 0;
    }
    offset = lseek(courseFileDescriptor, (courseID-1) * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return 0;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on course record!");
        return 0;
    }

    //Make it notactive to remove
    strcpy(course.status,"notactive");

    writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update course info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLKW, &lock);

    close(courseFileDescriptor);
 
    // make uneroll for those courses
    struct Enrollment enroll;
    int enrollfd;
    bool flag=false;
    int temp1[15][10];
    int i,n;
    int count=0;
    enrollfd = open(ENROLL_FILE,O_RDONLY);
    while((n = read(enrollfd, &enroll, sizeof(struct Enrollment))) > 0) {
       if((strcmp(enroll.status,"enrolled")==0)  && (strcmp(enroll.courseid,readBuffer)==0)){              
         temp1[i][10]= enroll.id;
         i++;
         count++;
         flag=true;  
       }
    }
    close(enrollfd);
    if(flag==true){
       for(int i=0;i<count;i++){
            enrollfd = open(ENROLL_FILE,O_RDONLY);
            int offset = lseek(enrollfd,(temp1[i][10]-1)*sizeof(struct Enrollment),SEEK_SET);
            if(offset == -1){
                perror("Error while seeking to required enrollment record!");
                return 0;
            }
            lock1.l_type = F_RDLCK;
            lock1.l_start = offset;
            int lockingStatus = fcntl(enrollfd, F_SETLKW, &lock1);
            if (lockingStatus == -1){
                perror("Error while obtaining read lock on enrollment record!");
                return 0;
            }
            readBytes = read(enrollfd, &enroll, sizeof(struct Enrollment));
            if(readBytes == -1){
                perror("Error while reading enrollment record from the file!");
                return 0;
            }
            lock1.l_type = F_UNLCK;
            lockingStatus = fcntl(enrollfd, F_SETLK, &lock1);   
            close(enrollfd);
            strcpy(enroll.status,"unenrolled");
  
            enrollfd = open(ENROLL_FILE,O_WRONLY);
            if (enrollfd == -1){
                perror("Error while opening enrollment file");
                return 0;
            }
            offset = lseek(enrollfd, (temp1[i][10]-1) * sizeof(struct Enrollment), SEEK_SET);
            if(offset == -1){
                perror("Error while seeking to required enrollment record!");
                return 0;
            }
            lock1.l_type = F_WRLCK;
            lock1.l_start = offset;
            lockingStatus = fcntl(enrollfd, F_SETLKW, &lock);
            if(lockingStatus == -1){
                perror("Error while obtaining write lock on enrollment record!");
                return 0;
            }
            writeBytes = write(enrollfd, &enroll, sizeof(struct Enrollment));
            if(writeBytes == -1){
                perror("Error while writing update enrollment info into file");
            }
            lock1.l_type = F_UNLCK;
            fcntl(enrollfd, F_SETLKW, &lock1);
            close(enrollfd);
       }
    }


    writeBytes = write(connFD, DEL_COURSE_SUCCESS, strlen(DEL_COURSE_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing DEL_COURSE_SUCCESS message to client!");
        return 0;
    }
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 1;
}

int compareEnrollmentTime(const void* a, const void* b) {
    const struct middleware* enrollA = (const struct middleware*)a;
    const struct middleware* enrollB = (const struct middleware*)b;
    
    char timeStrA[20];
    char timeStrB[20];

    strftime(timeStrA, sizeof(timeStrA), "%Y-%m-%d %H:%M", localtime(&(enrollA->mytime)));
    strftime(timeStrB, sizeof(timeStrB), "%Y-%m-%d %H:%M", localtime(&(enrollB->mytime)));

    // Compare the strings in reverse order (descending)
    return strcmp(timeStrB, timeStrA);
}
    
int modify_course(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;
    int courseID;
    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, MOD_COURSE_ID, strlen(MOD_COURSE_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing MOD_COURSE_ID message to client!");
        return 0;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading course ID from client!");
        return 0;
    }
    char *ftPosition = strstr(readBuffer, "C-");
    char *numberStart = NULL;
    if(ftPosition!=NULL) {
        // Move the pointer to the character right after "C-"
        numberStart = ftPosition + strlen("C-");
        // Convert the numeric part to an integer
        courseID = atoi(numberStart);
    }
    else{
        write(connFD,"wrong courseid ^",17);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }

    int courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Faculty File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer,"course id doesn't exists ^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing course id doesnt exists message to client!");
            return 0;
        }
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    
    offset = lseek(courseFileDescriptor,-sizeof(struct Course),SEEK_END);
    readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error reading course record from file!");
        return false;
    }
    if(courseID>course.id){
        write(connFD,"Invalid course id ^",20);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    close(courseFileDescriptor);
    courseFileDescriptor = open(COURSE_FILE,O_RDONLY);



    offset = lseek(courseFileDescriptor, (courseID-1)* sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // course record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer,"Course id doesn't exists ^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing course id doesnt exists message to client!");
            return 0;
        }
        readBytes =read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
    struct flock lock1 = {F_RDLCK,SEEK_SET,offset,sizeof(struct Enrollment),getpid()};
    // Lock the record to be read
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on course record!");
        return 0;
    }

    readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
    
    if(strcmp(course.status,"notactive")==0){
        write(connFD,"Invalid course id ^",20);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    int noofavailseats = course.no_of_available_seats;
    int noofseatsbefore = course.no_of_seats;
    if (readBytes == -1)
    {
        perror("Error while reading course record from the file!");
        return 0;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    close(courseFileDescriptor);
    if(strcmp(loggedInFaculty.loginid,course.facultyloginid)!=0){
        write(connFD,"Not your course to modify ^",27);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    writeBytes = write(connFD, MOD_COURSE_MENU, strlen(MOD_COURSE_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing MOD_COURSE_MENU message to client!");
        return 0;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting course modification menu choice from client!");
        return 0;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return 0;
        }
        return 0;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        writeBytes = write(connFD, MOD_COURSE_NEW_NAME, strlen(MOD_COURSE_NEW_NAME));
        if (writeBytes == -1)
        {
            perror("Error while writing MOD_COURSE_NEW_NAME message to client!");
            return 0;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for course's new name from client!");
            return 0;
        }
        strcpy(course.name, readBuffer);
        break;

    case 2:
        writeBytes = write(connFD, MOD_COURSE_NEW_DEPARTMENT, strlen(MOD_COURSE_NEW_DEPARTMENT));
        if (writeBytes == -1)
        {
            perror("Error while writing MOD_COURSE_NEW_DEPARTMENT message to client!");
            return 0;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for course's new department from client!");
            return 0;
        }
        
        strcpy(course.department,readBuffer);
        break;

    case 3:
        writeBytes = write(connFD, MOD_COURSE_NEW_NOOFSEATS, strlen(MOD_COURSE_NEW_NOOFSEATS));
        if (writeBytes == -1)
        {
            perror("Error while writing MOD_COURSE_NEW_NOOFSEATS message to client!");
            return 0;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for course's new noofseats from client!");
            return 0;
        }
        int value = atoi(readBuffer);
        if(value<0){
            write(connFD,"Invalid total seats ^",21);
            readBytes = read(connFD,readBuffer,sizeof(readBuffer));
            return 0;
        }
        if(value==0)
           course.no_of_available_seats=0;
        if(value>noofseatsbefore){
            course.no_of_available_seats= course.no_of_available_seats+(value-noofseatsbefore);
        }
        else if(value<noofseatsbefore){
            int temp = value-noofseatsbefore;
            int n;
            char unenroll_course[10];
            strcpy(unenroll_course,course.courseid); 
            if((course.no_of_available_seats+(temp))<0){
                int temp2 = -1*(course.no_of_available_seats+temp);
                course.no_of_available_seats=0;
                int i,size;
                struct Enrollment enroll1;
                int enrollfd = open(ENROLL_FILE,O_RDWR);
                while((n = read(enrollfd, &enroll1, sizeof(struct Enrollment)))>0){
                    if((strcmp(enroll1.status,"enrolled")==0) && (strcmp(enroll1.courseid,unenroll_course)==0)){
                       myDict[i].myid= enroll1.id;
                       myDict[i].mytime = enroll1.enroll_time;
                       size++;
                       i++;
                    }                                      
                }
                close(enrollfd);
                qsort(myDict,size, sizeof(struct middleware), compareEnrollmentTime);
                int a;
                while(a<temp2 && a<size){
                    int ID= myDict[a].myid;
                    struct Enrollment enroll;
                    enrollfd = open(ENROLL_FILE,O_RDONLY);
                    offset = lseek(enrollfd,(ID-1)*sizeof(struct Enrollment),SEEK_SET);
                    if(offset == -1){
                        perror("Error while seeking to required course record!");
                        return 0;
                    }
                    lock1.l_type = F_RDLCK;
                    lock1.l_start = offset;
                    lockingStatus = fcntl(enrollfd, F_SETLKW, &lock);
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

                    strcpy(enroll.status,"unenrolled");
                    enrollfd = open(ENROLL_FILE,O_WRONLY);
                    if (enrollfd == -1){
                        perror("Error while opening enrollment file");
                        return 0;
                    }
                    offset = lseek(enrollfd, (ID-1) * sizeof(struct Enrollment), SEEK_SET);
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
                     a++;
                }
            }
            else    
                course.no_of_available_seats=course.no_of_available_seats+(temp);
        }   
        course.no_of_seats= value;
        break;

    case 4:
        writeBytes = write(connFD, MOD_COURSE_NEW_CREDITS, strlen(MOD_COURSE_NEW_CREDITS));
        if (writeBytes == -1)
        {
            perror("Error while writing MOD_COURSE_NEW_CREDITS message to client!");
            return 0;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for course's new credits from client!");
            return 0;
        }
        course.credits=atoi(readBuffer);
        break;

    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, INVALID_MENU_CHOICE);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        return 0;
    }

    courseFileDescriptor = open(COURSE_FILE, O_WRONLY);
    if (courseFileDescriptor == -1)
    {
        perror("Error while opening course file");
        return 0;
    }
    offset = lseek(courseFileDescriptor, (courseID-1) * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return 0;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on course record!");
        return 0;
    }

    writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update course info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLKW, &lock);

    close(courseFileDescriptor);

    writeBytes = write(connFD, MOD_COURSE_SUCCESS, strlen(MOD_COURSE_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing MOD_COURSE_SUCCESS message to client!");
        return 0;
    }
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 1;    
}

int view_offering_course(int connFD){
    ssize_t readBytes, writeBytes;             
    char readBuffer[1000], writeBuffer[10000]; 
    char tempBuffer[1000];

    struct Course fetchcourse;
    int courseFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};

    writeBytes = write(connFD, GET_COURSE_ID, strlen(GET_COURSE_ID));
    if (writeBytes == -1){
        perror("Error while writing GET_COURSE_ID message to client!");
        return 0;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1){
        perror("Error getting course ID from client!");
        return 0;
    }
    char *ftPosition = strstr(readBuffer, "C-");
    char *numberStart = NULL;
    int courseID;
    if(ftPosition!=NULL) {
        // Move the pointer to the character right after "C-"
        numberStart = ftPosition + strlen("C-");
        // Convert the numeric part to an integer
        courseID = atoi(numberStart);
        
    }
    else{
        write(connFD,"wrong courseid ^",17);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    } 
    courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Course File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "course file id doesn't exists ^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes ==-1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }

    int offset = lseek(courseFileDescriptor,-sizeof(struct Course),SEEK_END);
    readBytes = read(courseFileDescriptor, &fetchcourse, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error reading course record from file!");
        return false;
    }
    if(courseID>fetchcourse.id){
        write(connFD,"Invalid course id ^",20);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    close(courseFileDescriptor);
    courseFileDescriptor = open(COURSE_FILE,O_RDONLY);

    offset = lseek(courseFileDescriptor, (courseID-1) * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // Course record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "Course id doesn't exists $");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        return 0;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }
    lock.l_start = offset;

    int lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Course file!");
        return false;
    }

    readBytes = read(courseFileDescriptor, &fetchcourse, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error reading course record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    bzero(writeBuffer, sizeof(writeBuffer));
    if(strcmp(fetchcourse.facultyloginid,loggedInFaculty.loginid)!=0){
        write(connFD,NOT_YOUR_COURSE,strlen(NOT_YOUR_COURSE));
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    else if(strcmp(fetchcourse.status,"notactive")==0){
        write(connFD,"No course found ^",18);
        readBytes = read(connFD,readBuffer,sizeof(readBuffer));
        return 0;
    }
    sprintf(writeBuffer, "********* Course Details *********  \n\tName: %s\n\tDepartment : %s\n\tNo of Seats: %d\n\tCredits : %d\n\tNo of available seats: %d\n\tCourse-id: %s", fetchcourse.name, fetchcourse.department,fetchcourse.no_of_seats,fetchcourse.credits,fetchcourse.no_of_available_seats,fetchcourse.courseid);

    strcat(writeBuffer, "\n\nYou'll now be redirected to the Faculty menu ^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing course info to client!");
        return 0;
    }
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return true;
}

int logout(int connFD){
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];
    write(connFD,LOG_OUT,strlen(LOG_OUT));
    close(connFD);
    return 0;
}

int change_password(int connFD){
   ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000], hashedPassword[1000];

    char newPassword[1000];

    // Lock the critical section
    struct sembuf semOp = {0, -1, SEM_UNDO};
    int semopStatus = semop(semid, &semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
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

    if (strcmp(crypt(readBuffer, SALT_BAE), loggedInFaculty.password) == 0)
    {
        // Password matches with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password response from client");
            unlock_critical_section(&semOp);
            return false;
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
            return false;
        }

        if (strcmp(crypt(readBuffer, SALT_BAE), newPassword) == 0)
        {
            // New & reentered passwords match

            strcpy(loggedInFaculty.password, newPassword);

            int facultyFileDescriptor = open(FACULTY_FILE, O_WRONLY);
            if (facultyFileDescriptor == -1)
            {
                perror("Error opening faculty file!");
                unlock_critical_section(&semOp);
                return false;
            }

            off_t offset = lseek(facultyFileDescriptor, (loggedInFaculty.id-1) * sizeof(struct Faculty), SEEK_SET);
            if (offset == -1)
            {
                perror("Error seeking to the faculty record!");
                unlock_critical_section(&semOp);
                return false;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
            int lockingStatus = fcntl(facultyFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining write lock on faculty record!");
                unlock_critical_section(&semOp);
                return false;
            }

            writeBytes = write(facultyFileDescriptor, &loggedInFaculty, sizeof(struct Faculty));
            if (writeBytes == -1)
            {
                perror("Error storing updated faculty password into faculty record!");
                unlock_critical_section(&semOp);
                return false;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(facultyFileDescriptor, F_SETLK, &lock);

            close(facultyFileDescriptor);

            writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            unlock_critical_section(&semOp);

            return true;
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

bool unlock_critical_section(struct sembuf *semOp)
{
    semOp->sem_op = 1;
    int semopStatus = semop(semid, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while operating on semaphore!");
        _exit(1);
    }
    return true;
}

bool lock_critical_section(struct sembuf *semOp)
{
    semOp->sem_flg = SEM_UNDO;
    semOp->sem_op = -1;
    semOp->sem_num = 0;
    int semopStatus = semop(semid, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
    }
    return true;
}

int add_course(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course newCourse;
    struct Course prevCourse;

    int courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1 && errno == ENOENT)
    {
        // Course file was never created
        newCourse.id =1;
    }
    else if (courseFileDescriptor == -1)
    {
        perror("Error while opening course file");
        return 0;
    }
    else
    {
        int offset = lseek(courseFileDescriptor, -sizeof(struct Course), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Course record!");
            return 0;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
        int lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Course record!");
            return 0;
        }

        readBytes = read(courseFileDescriptor, &prevCourse, sizeof(struct Course));
        if (readBytes == -1)
        {
            perror("Error while reading Course record from file!");
            return 0;
        }

        lock.l_type = F_UNLCK;
        fcntl(courseFileDescriptor, F_SETLK, &lock);

        close(courseFileDescriptor);

        newCourse.id = prevCourse.id+1;
    }
    // course name 
    writeBytes = write(connFD, ADD_COURSE_NAME,strlen(ADD_COURSE_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing ADD_COURSE_NAME message to client!");
        return 0;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading course name from client!");
        return 0;
    }
    strcpy(newCourse.name,readBuffer);

    // course department 
    writeBytes = write(connFD, ADD_COURSE_DEPARTMENT,strlen(ADD_COURSE_DEPARTMENT));
    if (writeBytes == -1)
    {
        perror("Error writing ADD_COURSE_DEPARTMENT message to client!");
        return 0;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading course department  from client!");
        return 0;
    }
    strcpy(newCourse.department,readBuffer);

    // no of seats
    writeBytes = write(connFD, ADD_COURSE_SEATS,strlen(ADD_COURSE_SEATS));
    if (writeBytes == -1)
    {
        perror("Error writing ADD_COURSE_SEATS message to client!");
        return 0;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading no of seats from client!");
        return 0;
    }
    int seats = atoi(readBuffer);
    newCourse.no_of_seats= seats;

    // credits
    writeBytes = write(connFD, ADD_COURSE_CREDITS,strlen(ADD_COURSE_CREDITS));
    if (writeBytes == -1)
    {
        perror("Error writing ADD_COURSE_CREDITS message to client!");
        return 0;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading credits from client!");
        return 0;
    }
    int credits = atoi(readBuffer);
    newCourse.credits= credits;
     
    // no of available seats
    newCourse.no_of_available_seats=seats;

    //courseid
    char y[4];
    strcpy(newCourse.courseid,"C-");
    sprintf(y ,"%d",newCourse.id);
    strcat(newCourse.courseid, y);

    //course faculty name
    strcpy(newCourse.facultyloginid,loggedInFaculty.loginid);

    //course status
    strcpy(newCourse.status,"active");
    courseFileDescriptor = open(COURSE_FILE, O_CREAT|O_APPEND|O_WRONLY,S_IRWXU);
    if (courseFileDescriptor == -1)
    {
        perror("Error while creating / opening course file!");
        return 0;
    }

    writeBytes = write(courseFileDescriptor, &newCourse, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing Course record to file!");
        return 0;
    }

    close(courseFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "%s%s%d", ADD_COURSE_SUCCESS, newCourse.courseid,newCourse.id);
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    
    readBytes = read(connFD,readBuffer,sizeof(readBuffer));
    return 1;
}

#endif