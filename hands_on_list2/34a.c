/*
============================================================================
Name : 34a.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to create a concurrent server.
            a. use fork
Date: 1st Oct 2023.
============================================================================
*/


#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>

int handle_client(int client_socket,int i) {
    char buffer[1024];
    char response[1024];
    int bytes_received;

    // Receive data from the client
    bytes_received = read(client_socket,buffer,sizeof(buffer));
    if(bytes_received<0){
      perror("recv returns");
      return 1;
    }

    printf("Received from client %d: %s\n",i,buffer);
    printf("Enter message for client%d:",i);
    scanf("%[^\n]s",response);
    write(client_socket,response,sizeof(response));

    // Close the client socket
    close(client_socket);
    return 1;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_addr_len = sizeof(client_address);

    // Create a socket
    server_socket = socket(AF_INET,SOCK_STREAM,0);
    if(server_socket<0){
       perror("socket returns");
       return 1;
    }

   // memset(&server_address, 0, sizeof(server_address));
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

    printf("Server listening for connections....\n");
    int number=1;
    
    while(1){
      // Accept a client connection
      client_socket = accept(server_socket,(struct sockaddr*)&client_address,&client_addr_len);
      if(client_socket<0){
         perror("accept returns");
         continue; // Continue to accept other connections
      }

      // Fork a child process to handle the client
      pid_t child_pid = fork();
      if(child_pid == 0){
         close(server_socket); // Close the server socket in the child process
         handle_client(client_socket,number); // Handle the client
         number=number+1;
         return 1; 
      } 
      else if(child_pid>0){
        close(client_socket); // Close the client socket in the parent process
      } 
      else{
        perror("fork returns");
        close(client_socket); // Close the client socket in case of fork failure
      }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}

