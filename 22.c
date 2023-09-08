/*
============================================================================
Name : 22.c
Author : G.Sai Hemanth Kumar 
Description : 
        Write a program, open a file, call fork, and then write to the file by both the child as well as the parent processes. Check output of the file.          
Date: 1st Sept, 2023.
============================================================================
*/


#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc, char* argv[]){
    if(argc!=2){
      printf("missing arguments");
      return 1;
    }
    
    int fd = open(argv[1],O_CREAT|O_RDWR|O_TRUNC,0644);
    if(fd==-1){
      perror("Error opening a file");
      return 1;
    }
    
    if(fork()){
      char a[] = "Written by parent process";
      int fd_parent =write(fd,a,sizeof(a));
      if(fd_parent==-1){
        perror("Error writing to a file");
        return 1;
       }  
    }
    
    else{
      char a[] = "Written by child process";
      int fd_child =write(fd,a,sizeof(a));
      if(fd_child==-1){
        perror("Error writing to a file");
        return 1;
       }
    }  
    
    int fd_close = close(fd);
    if(fd_close==-1){
      perror("Error closing the file");
      return 1;
    }
   return 1; 
}
