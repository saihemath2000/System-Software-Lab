/*
============================================================================
Name : 12.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to find out the opening mode of a file. Use fcntl.             
Date: 25th Aug, 2023.
============================================================================
*/



#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>

int main(int argc, char* argv[]){
   if(argc!=2){
     printf("missing arguments");
     return 1;
   }
   
   int fd = open(argv[1],O_CREAT|O_RDONLY|O_TRUNC,0644);
   if(fd==-1){
     perror("Error opening a file");
     return 1;
   }
 
   int flags, accessMode;
   flags = fcntl(fd, F_GETFL);
   if(flags==-1){
      printf("problem reading access modes");
      return 1;
   }
      
   // Checking the access mode of the file is slightly more complex, since the O_RDONLY (0), O_WRONLY (1), and O_RDWR (2)     
   // constants don’t correspond to single bits in the open file status flags. 
   // Therefore, to make this check, we mask the flags value with the constant O_ACCMODE, and then test for equality
      
   accessMode = flags & O_ACCMODE;   
   if(accessMode == O_WRONLY || accessMode == O_RDWR)
       printf("file is writable\n");
   if(accessMode == O_RDONLY || accessMode == O_RDWR)
       printf("file is readable\n");
       
   int close_fd = close(fd);
   if(close_fd==-1){
     perror("Error closing a file");
     return 1;
   }        
   return 1;
}
