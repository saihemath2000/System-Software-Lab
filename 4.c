#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

 int main(int argc, char *argv[]){
     if(argc!=2){
       printf("Missing arguments");
       return 1;
     }
     
     //opening a file
     int fd = open(argv[1],O_CREAT|O_EXCL,0644);
     if(fd==-1){
       perror("Error opening a file");
       return 1;
     }
     
     printf("fd=%d",fd);
     close(fd);
     return 1;
 }
