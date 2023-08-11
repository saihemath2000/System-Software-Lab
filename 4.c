#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
 int main(void){
     int fd;
     fd = open("program4",O_EXCL|O_RDWR,0744);
     printf("fd=%d",fd);
     perror("fd returns:");
     close(fd);
     return 1;
 }
