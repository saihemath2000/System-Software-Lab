#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include "./common.h"

int admin_operation_handler(int connFD);
int add_student(int connFD);
int admin_operation_handler(int connFD)
{
    if(login_handler(1,connFD,NULL))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, ADMIN_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ADMIN_MENU to client!");
                return 0;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return 0;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                add_student(connFD);
                break;
            // case 2:
            //     view_student_details(connFD, NULL);
            //     break;
            // case 3: 
            //     add_faculty(connFD, -1);
            //     break;
            // case 4:
            //     view_student_details(connFD);
            //     break;
            // case 5:
            //     activate_student(connFD);
            //     break;
            // case 6:
            //     block_student(connFD);
            //     break;
            // case 7:
            //     modify_student_details(connFD);
            //     break;
            // case 8:
            //     modify_faculty_details(connFD);
            //     break;
            // case 9:
            //     logout(connFD);
            //     break;            
            default:
                // writeBytes = write(connFD, ADMIN_LOGOUT, strlen(ADMIN_LOGOUT));
                return 0;
            }
        }
    }
    else
    {
        // ADMIN LOGIN FAILED
        return 0;
    }
    return 1;
}

int add_student(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Student newStudent, previousStudent;

    int studentFileDescriptor = open(STUDENT_FILE, O_RDONLY);
    if (studentFileDescriptor == -1 && errno == ENOENT)
    {
        // Customer file was never created
        newStudent.id = 0;
    }
    else if (studentFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return -1;
    }
    else
    {
        int offset = lseek(studentFileDescriptor, -sizeof(struct Student), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last student record!");
            return 0;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};
        int lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on student record!");
            return 0;
        }

        readBytes = read(studentFileDescriptor, &previousStudent, sizeof(struct Student));
        if (readBytes == -1)
        {
            perror("Error while reading Student record from file!");
            return 0;
        }

        lock.l_type = F_UNLCK;
        fcntl(studentFileDescriptor, F_SETLK, &lock);

        close(studentFileDescriptor);

        newStudent.id = previousStudent.id + 1;
    }

    printf("Enter name:\n");
    writeBytes = write(0,writeBuffer,sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing Student name!");
        return 0;
    }
    strcpy(newStudent.name, writeBuffer);

    bzero(writeBuffer, sizeof(writeBuffer));

    printf("Enter age:\n");
    int temp1;
    writeBytes = write(0,&temp1,sizeof(temp1));
    if (writeBytes == -1)
    {
        perror("Error writing Student age!");
        return 0;
    }
    newStudent.age=temp1;
    
    printf("Enter address:\n");
    writeBytes = write(0,writeBuffer,sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing Student address!");
        return 0;
    }
    strcpy(newStudent.address, writeBuffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    
    printf("Enter email:\n");
    writeBytes = write(0,writeBuffer,sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing Student email!");
        return 0;
    }
    strcpy(newStudent.email, writeBuffer);
    
    bzero(writeBuffer, sizeof(writeBuffer));

    //loginid as name+"-"+system_id    
    strcpy(newStudent.loginid, newStudent.name);
    strcat(newStudent.loginid, "-");
    sprintf(writeBuffer, "%d", newStudent.id);
    strcat(newStudent.loginid, writeBuffer);

    char hashedPassword[1000];
    strcpy(hashedPassword, crypt(AUTOGEN_PASSWORD, SALT_BAE));
    strcpy(newStudent.password, hashedPassword);

    studentFileDescriptor = open(STUDENT_FILE, O_CREAT|O_APPEND|O_WRONLY,S_IRWXU);
    if (studentFileDescriptor == -1)
    {
        perror("Error while creating / opening student file!");
        return 0;
    }
    writeBytes = write(studentFileDescriptor, &newStudent, sizeof(newStudent));
    if (writeBytes == -1)
    {
        perror("Error while writing Student record to file!");
        return 0;
    }

    close(studentFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    printf("Student added successfully!\n");
    sprintf(writeBuffer, "%s%s-%d\n", ADMIN_ADD_STUDENT_AUTOGEN_LOGIN, newStudent.name, newStudent.id);
    strcat(writeBuffer, "^");
    writeBytes = write(1, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error displaying login details");
        return 0;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return newStudent.id;
}

#endif