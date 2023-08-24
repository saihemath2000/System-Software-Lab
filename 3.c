#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
 int main(void){
     int fd;
     
     fd = creat("program3",0744);
     if(fd==-1){
       perror("error in creating a file:");
       return 0;
     }
     else{
       printf("fd=%d",fd);
     }
     
     close(fd);
     return 1;
 }
