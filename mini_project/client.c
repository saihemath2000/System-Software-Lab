#include <sys/socket.h> 
#include <netinet/ip.h> // Import for `struct sockaddr_in`, `htons`
#include <stdio.h>      
#include <unistd.h>
#include <string.h>  

void connection_handler(int connFD);
void main(){
    int socketFileDescriptor; 
    int connectStatus; 

    struct sockaddr_in address;

    ssize_t readBytes, writeBytes;
    char dataFromServer[1024];
    int choice;
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
   
    connection_handler(socketFileDescriptor);
    close(socketFileDescriptor);
}

void connection_handler(int sockFD)
{
    char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading from / writting to the server
    ssize_t readBytes, writeBytes;            // Number of bytes read from / written to the socket

    char tempBuffer[1000];

    do
    {
        bzero(readBuffer, sizeof(readBuffer)); // Empty the read buffer
        bzero(tempBuffer, sizeof(tempBuffer));
        readBytes = read(sockFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
            perror("Error while reading from client socket!");
        else if (readBytes == 0)
            printf("No message received from server! Closing the connection to the server now!\n");
        else
        {
            printf("%s\n", readBuffer);
            scanf("%[^\n]%*c", writeBuffer); // Take user input!
            writeBytes = write(sockFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1){
                perror("Error while writing to client socket!");
                printf("Closing the connection to the server now!\n");
                break;
            }
        }
    } while(readBytes > 0);

    close(sockFD);
}

    // readBytes = read(socketFileDescriptor, dataFromServer, 134);
    // if(readBytes == -1)
    //    perror("Error while reading from network via socket!");
    // else
    //    printf("%s\n", dataFromServer);
    // scanf("%d",&choice);

    // writeBytes = write(socketFileDescriptor,&choice,1);
    // if (writeBytes == -1)
    //     perror("Error while writing to network via socket!");

    