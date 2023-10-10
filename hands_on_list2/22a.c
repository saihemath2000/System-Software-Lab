


/*
============================================================================
Name : 22a.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to wait for data to be written into FIFO within 10 seconds, use select system call with FIFO.
         In this file we are reading from FIFO file
Date: 30th Sept 2023.
============================================================================
*/


#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/select.h>
#include<sys/time.h>

int main() {
  char buff[20];
  
  int fd = open("myfifo", O_NONBLOCK|O_RDONLY);
  if(fd==-1){
    printf("Unable to open file\n");
    return 1;
  }
  
  fd_set rfds;  
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  struct timeval tv;
  tv.tv_sec = 10;
  tv.tv_usec=0;
  
  int var = select(fd+1,&rfds,NULL, NULL,&tv);
  if(var==-1){
    printf("Error in select");
    return 1;
  }    
  if(var==0)
    printf("No data available within 10 seconds");
  else{
      printf("Data is available now\n");
      int fd_read = read(fd, buff, sizeof(buff));
      if(fd_read==-1){
        perror("returns:");
        close(fd);
        return 1;
      }
      printf("%s\n", buff);
  }
  close(fd);
  return 1;
}
