/*
============================================================================
Name : 10.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to open a file with read write mode, write 10 bytes, move the file pointer by 10 bytes (use lseek) and write again 10 bytes.
          a. check the return value of lseek
          b. open the file with od and check the empty spaces in between the data.             
Date: 18th Aug, 2023.
============================================================================
*/



#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
int main(int argc, char* argv[]){

  if(argc!=3){
     printf("missing arguments");
     return 1;
  }
  char a[10];
  strncpy(a,argv[2],sizeof(a)); 
  
  // open file and create if it doesnt exists
  int fd = open(argv[1],O_RDWR|O_CREAT|O_TRUNC, 0644);
  if(fd==-1)
    perror("file returns");
  
  // writing 10 bytes into the file  
  ssize_t bytes = write(fd,a,sizeof(a)-1);
  if(bytes==-1){
     perror("error writing data");
     close(fd);
     return 1;
  }
  
  // move the file pointer by 10 bytes from the curr position
  off_t curr = lseek(fd,10,SEEK_CUR);
  if(curr==-1){
    perror("error seeking file pointer");
    close(fd);
    return 1;
  }
  
  // a. Printing lseek returned value
  printf("Written value of lseek is: %ld",(long int)curr);
  printf("\n");
  
  // write 10 bytes again into the file
  bytes = write(fd,a,sizeof(a)-1);
  if(bytes==-1){
     perror("error writing data");
     close(fd);
     return 1;
  }
  
  //closing the file
  int fd_close = close(fd);
  if(fd_close==-1){
    perror("error closing the file");
    return 1;
  }
  
  printf("Data written and file pointer moved successfully");
  return 1;
  
  // b. "od -c file1"  to check the empty spaces in between the data.
}
