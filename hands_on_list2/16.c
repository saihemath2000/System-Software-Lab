/*
============================================================================
Name : 16.c
Author : G.Sai Hemanth Kumar
Description : Write a program to send and receive data from parent to child vice versa. Use two way communication.
Date: 30th Sept 2023.
============================================================================
*/


#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
  char buff1[50];
  char buff2[50];
  int fd1[2];
  int fd2[2];
  pipe(fd1);
  pipe(fd2);
  
  if(!fork()){
     close(fd1[0]); // Close read-end of fd1 for child
     close(fd2[1]); // Close write-end of fd2 for child
     
     printf("Enter message to parent:");
     scanf("%[^\n]", buff1);
     
     int fd_write = write(fd1[1], buff1, sizeof(buff1));
     if(fd_write==-1){
       perror("returns:");
       close(fd1[1]);
       return 1;
     }
     
     int fd_read = read(fd2[0], buff2, sizeof(buff2));
     if(fd_read==-1){
       perror("returns:");
       close(fd2[0]);
       return 1;
     }
     
     printf("Message from parent: %s\n", buff2);
  }
  else {
     close(fd1[1]); // Close read-end of fd1 for parent
     close(fd2[0]); // Close read-end of fd2 for parent

     int fd_read = read(fd1[0], buff1, sizeof(buff1));
     if(fd_read==-1){
       perror("returns:");
       close(fd1[0]);
       return 1;
     }
     
     printf("Message from child: %s\n", buff1);
     printf("Enter message to child: ");
     scanf(" %[^\n]", buff2);
     
     int fd_write = write(fd2[1], buff2, sizeof(buff2));
     if(fd_write==-1){
       perror("returns:");
       close(fd2[1]);
       return 1;
     }
  }
  
  close(fd1[0]);
  close(fd2[1]);
  return 1; 
}
