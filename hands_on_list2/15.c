/*
============================================================================
Name : 15.c
Author : G.Sai Hemanth Kumar
Description : Write a simple program to send some data from parent to the child process.
Date: 30th Sept 2023.
============================================================================
*/


#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>

int main() {
  char buff[80];
  int fd[2];
  pipe(fd);
  if(fork()){
     close(fd[0]); // Close read-end of pipe for parent
     printf("Enter message to the child: ");
     scanf(" %[^\n]", buff); 
     write(fd[1], buff, sizeof(buff));
  }
  else{
    close(fd[1]); // Close write-end of pipe for child
    read(fd[0], buff, sizeof(buff));
    printf("Message from parent: %s\n", buff);
  }
  wait(0); // Wait till all children have terminated
}
