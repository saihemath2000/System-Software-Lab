/*
============================================================================
Name : 14.c
Author : G.Sai Hemanth Kumar
Description : Write a simple program to create a pipe, write to the pipe, read from pipe and display on
              the monitor.
Date: 30th Sept 2023.
============================================================================
*/


#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
   int fd[2];
   char write_buff[50];
   char read_buff[50];
   
   if(pipe(fd)==-1){
     printf("Error creating a pipe\n");
   }
   
   printf("Enter message:\n");
   scanf("%[^\n]s",write_buff);
   
   size_t bytes_written = write(fd[1],write_buff,sizeof(write_buff));  //writing into pipe
   if(bytes_written==-1){
     perror("returns:");
     close(fd[1]);
     return 1;
   }
   
   int fd_read = read(fd[0],read_buff,sizeof(read_buff));  //reading from pipe
   if(fd_read==-1){
     perror("returns");
     close(fd[0]);
     return 1;
   }
   
   printf("Message received from pipe:%s\n",read_buff);
   close(fd[1]);
   close(fd[0]);
   return 1;
}
