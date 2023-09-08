/*
============================================================================
Name : 17.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to simulate online ticket reservation. Implement write lock Write a program to open a file, 
           store a ticket number and exit.
Date: 27th Aug, 2023.
============================================================================
*/



#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>

struct {
  int ticket_no;
}db;

int main(int argc, char *argv[]){
   if(argc!=2){
      printf("missing arguments");
      return 1;
   }
   
   db.ticket_no=5;
   
   int fd = open(argv[1],O_CREAT|O_RDWR,0644);
   if(fd==-1){
     perror("Error opening a file");
     return 1;
   }
       
   int fd_write = write(fd,&db,sizeof(db));
   if(fd_write==-1){
     perror("Error writing into the file");
     close(fd);
     return 1;
   }
   close(fd);
   
   fd = open(argv[1],O_RDONLY);
   int fd_read = read(fd,&db,sizeof(db));
   if(fd_read==-1){
     perror("Error reading the file");
     close(fd);
     return 1;
   }
   
   printf("\nTicket no is:%d",db.ticket_no);
   close(fd);    
}
