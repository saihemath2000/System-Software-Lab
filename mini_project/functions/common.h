#include <stdio.h>     
#include <unistd.h>    
#include <string.h>    
#include <stdbool.h>   
#include <sys/types.h> 
#include <sys/stat.h>  
#include <fcntl.h>     
#include <stdlib.h>    
#include <errno.h>     
#include <crypt.h>

#include "./server-constants.h"
#include "./admin-credentials.h"
#include "../record-struct/faculty.h"
#include "../record-struct/student.h"
#include "../record-struct/course.h"

bool login_handler(int user, int connFD, struct Faculty *ptrToFacultyID);

bool login_handler(int user, int connFD, struct Faculty *ptrToFacultyID)
{
    ssize_t readBytes, writeBytes;            
    char readBuffer[1000], writeBuffer[1000]; 
    char tempBuffer[1000];
    struct Faculty faculty;
    struct Student student;

    int ID;

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    strcat(writeBuffer, "\n");
    strcat(writeBuffer, LOGIN_ID);

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing WELCOME & LOGIN_ID message to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading login ID from client!");
        return false;
    }

    bool userFound = false;

    if (user==1)
    {
        if (strcmp(readBuffer, ADMIN_LOGIN_ID) == 0)
            userFound = true;
    }
    else if(user==2)
    {
        bzero(tempBuffer, sizeof(tempBuffer));
        strcpy(tempBuffer, readBuffer);
        int id;
        char *numberStart = NULL;
        // Find the position of "FT-" in the string
        char *ftPosition = strstr(tempBuffer,"FT-");
        if (ftPosition != NULL) {
           // Move the pointer to the character right after "FT-"
           numberStart = ftPosition + strlen("FT-");
           id = atoi(numberStart);
        } 

        int facultyFileFD = open(FACULTY_FILE, O_RDONLY);
        if (facultyFileFD == -1)
        {
            perror("Error opening faculty file in read mode!");
            return false;
        }

        off_t offset = lseek(facultyFileFD,(id-1) * sizeof(struct Faculty), SEEK_SET);
        if (offset >= 0)
        {
            struct flock lock = {F_RDLCK, SEEK_SET, (id-1) * sizeof(struct Faculty), sizeof(struct Faculty), getpid()};

            int lockingStatus = fcntl(facultyFileFD, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining read lock on faculty record!");
                return false;
            }

            readBytes = read(facultyFileFD, &faculty, sizeof(struct Faculty));
            if (readBytes == -1)
            {
                perror("Error reading faculty record from file!");
            }

            lock.l_type = F_UNLCK;
            fcntl(facultyFileFD, F_SETLK, &lock);

            if (strcmp(faculty.loginid, readBuffer) == 0)
                userFound = true;

            close(facultyFileFD);
        }
        else
        {
            writeBytes = write(connFD, FACULTY_LOGIN_ID_DOESNT_EXIT, strlen(FACULTY_LOGIN_ID_DOESNT_EXIT));
        }
    }

    if(userFound)
    {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, PASSWORD, strlen(PASSWORD));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == 1)
        {
            perror("Error reading password from the client!");
            return false;
        }
          
        char hashedPassword[1000];
        strcpy(hashedPassword, crypt(readBuffer, SALT_BAE));
        if (user==1)
        {
            if (strcmp(readBuffer, ADMIN_PASSWORD) == 0)
                return true;
        }
        else
        {
            //66pXmLtsArbAk
            if (strcmp(hashedPassword, faculty.password) == 0)
            {
                *ptrToFacultyID = faculty;
                return true;
            }
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, INVALID_PASSWORD, strlen(INVALID_PASSWORD));
    }
    else
    {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, INVALID_LOGIN, strlen(INVALID_LOGIN));
    }

    return false;
}