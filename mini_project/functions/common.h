#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>    // Import for string functions
#include <stdbool.h>   // Import for `bool` data type
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`
#include <crypt.h>

#include "./server-constants.h"
#include "./admin-credentials.h"
#include "../record-struct/faculty.h"
#include "../record-struct/student.h"
bool login_handler(int user, int connFD, struct Student *ptrToCustomerID)
{
    ssize_t readBytes, writeBytes;            // Number of bytes written to / read from the socket
    char readBuffer[1000], writeBuffer[1000]; // Buffer for reading from / writing to the client
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
    // else if(user==2)
    // {
    //     bzero(tempBuffer, sizeof(tempBuffer));
    //     strcpy(tempBuffer, readBuffer);
    //     strtok(tempBuffer, " ");
    //     ID = atoi(strtok(NULL, " "));

    //     int facultyFileFD = open(FACULTY_FILE, O_RDONLY);
    //     if (facultyFileFD == -1)
    //     {
    //         perror("Error opening faculty file in read mode!");
    //         return false;
    //     }

    //     off_t offset = lseek(facultyFileFD,ID * sizeof(struct Faculty), SEEK_SET);
    //     if (offset >= 0)
    //     {
    //         struct flock lock = {F_RDLCK, SEEK_SET, ID * sizeof(struct Faculty), sizeof(struct Faculty), getpid()};

    //         int lockingStatus = fcntl(facultyFileFD, F_SETLKW, &lock);
    //         if (lockingStatus == -1)
    //         {
    //             perror("Error obtaining read lock on customer record!");
    //             return false;
    //         }

    //         readBytes = read(facultyFileFD, &faculty, sizeof(struct Faculty));
    //         if (readBytes == -1)
    //         {
    //             ;
    //             perror("Error reading customer record from file!");
    //         }

    //         lock.l_type = F_UNLCK;
    //         fcntl(customerFileFD, F_SETLK, &lock);

    //         if (strcmp(customer.login, readBuffer) == 0)
    //             userFound = true;

    //         close(customerFileFD);
    //     }
    //     else
    //     {
    //         writeBytes = write(connFD, CUSTOMER_LOGIN_ID_DOESNT_EXIT, strlen(CUSTOMER_LOGIN_ID_DOESNT_EXIT));
    //     }
    // }

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
        // strcpy(hashedPassword, crypt(readBuffer, SALT_BAE));
        strcpy(hashedPassword,readBuffer);
        if (user==1)
        {
            if (strcmp(hashedPassword, ADMIN_PASSWORD) == 0)
                return true;
        }
        // else
        // {
        //     if (strcmp(hashedPassword, customer.password) == 0)
        //     {
        //         *ptrToCustomerID = customer;
        //         return true;
        //     }
        // }

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