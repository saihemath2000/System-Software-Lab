/*
============================================================================
Name : 8.c
Author : G.Sai Hemanth Kumar 
Description : 
             Write a program to open a file in read only mode, read line by line and display each line as it is read. Close the file when end of file is reached.
Date: 18th Aug, 2023.
============================================================================
*/



#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char* argv[]){

  //checking error for arguments
  if(argc!=2){
    printf("missing arguments");
    return 1;
  }

  //opening file and checking error
  int fd_open = open(argv[1],O_RDONLY);
  if(fd_open==-1) 
     perror("returns:");
     
  int i=0;
  while(1){
     char buf;
     char temp[1024];
     int char_read = read(fd_open,&buf,1);
     if(char_read==-1){
       perror("returns:");
     }
     
     if(buf=='\n'){
        write(1,temp,strlen(temp));
        memset(temp,'\0',sizeof(temp));
        i=0;
     }
     
     if(char_read==0){
       close(fd_open);
       break;
     }
     temp[i++]=buf;
  }
  return 1;
}
