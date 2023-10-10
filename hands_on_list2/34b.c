/*
============================================================================
Name : 34b.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to create a concurrent server.
            b. use pthread_create
Date: 1st Oct 2023.
============================================================================
*/


#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>

// Function to handle each client in a separate thread

void *handle_client(void *client_socket_ptr) {
    int client_socket = *((int *)client_socket_ptr);
    free(client_socket_ptr); // Free the memory allocated for the client_socket

    char buffer[1024];
    char response[1024];
    int bytes_received;

    // Receive data from the client
    bytes_received = read(client_socket,buffer,sizeof(buffer));
    if (bytes_received < 0) {
        perror("recv returns");
        close(client_socket);
        pthread_exit(NULL);
    }
    
    printf("Received from client: %s\n", buffer);
    printf("Enter message for client:\n");
    read(0,response,sizeof(response));
    //scanf("%[^\n]s",response); 
    write(client_socket, response, sizeof(response));

    close(client_socket);
    pthread_exit(NULL);
}

int main(){
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_addr_len = sizeof(client_address);
    pthread_t thread_id;

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket<0){
      perror("socket returns");
      return 1;
    }

    //memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080); 

    // Bind the socket to the server address
    if(bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0){
       perror("bind returns");
       close(server_socket);
       return 1;
    }

    // Listen for incoming connections
    if(listen(server_socket,5)<0){
       perror("listen returns");
       close(server_socket);
       return 1;
    }
    printf("Server is listening for connections....\n");

    while(1){
      // Accept a client connection
      client_socket = accept(server_socket,(struct sockaddr*) &client_address,&client_addr_len);
      if(client_socket<0){
        perror("accept returns");
        continue; // Continue to accept other connections
      }
       
      // Allocate memory for the client socket to pass to the thread
      int *client_socket_ptr = (int *)malloc(sizeof(int));
      if(client_socket_ptr==NULL){
         perror("malloc returns");
         close(client_socket);
         continue;
      }
      *client_socket_ptr = client_socket;

      // Create a new thread to handle the client
      if(pthread_create(&thread_id,NULL,handle_client,client_socket_ptr)!=0){
         perror("pthread_create");
         free(client_socket_ptr);
         close(client_socket);
      }  
      
      // Detach the thread to avoid resource leak
      pthread_detach(thread_id);
    }

    // Close the server socket
    close(server_socket);
    return 0;
}

