/*
============================================================================
Name : 33_client.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to communicate between two machines using socket.
Date: 1st Oct 2023.
============================================================================
*/


#include <sys/socket.h> 
#include <netinet/ip.h> // Import for `struct sockaddr_in`, `htons`
#include <stdio.h>      
#include <unistd.h>  

void main(){
    int socketFileDescriptor; 
    int connectStatus; 

    struct sockaddr_in address;

    ssize_t readBytes, writeBytes;
    char dataFromServer[1024];
    char dataToServer[1024];
    // Create an endpoint for communicate -> here, create the client side point
    // Create a socket for TCP connection using IPv4
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor == -1)
    {
        perror("Error while creating socket!");
        _exit(0);
    }

    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    connectStatus = connect(socketFileDescriptor, (struct sockaddr *)&address, sizeof(address));
    if (connectStatus == -1)
    {
        perror("Error while connecting to server!");
        _exit(0);
    }
    printf("Client to server connection successfully established!\n");

    // ========================= Client - Server communication =================

    printf("Enter message for server:\n");
    scanf("%[^\n]s",dataToServer);
    writeBytes = write(socketFileDescriptor,dataToServer,sizeof(dataToServer));
    if (writeBytes == -1)
        perror("Error while writing to network via socket!");

    readBytes = read(socketFileDescriptor, dataFromServer, 100);
    if (readBytes == -1)
        perror("Error while reading from network via socket!");
    else
        printf("Message from server: %s\n", dataFromServer);
     

    // =======================================================================

    close(socketFileDescriptor);
}
