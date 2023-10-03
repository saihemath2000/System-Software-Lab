/*
============================================================================
Name : 17_1.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to simulate online ticket reservation. Implement write lock.
           Write a separate program, to open the file, implement write lock, read the ticket number, increment the number and  print the new ticket number then close the file.

Date: 27th Aug, 2023.
============================================================================
*/


#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>

struct {
   int ticket_no;
}db;

int main(){

    struct flock lock;
    
    //setting lock attributes
    lock.l_type=F_WRLCK;
    lock.l_whence= SEEK_SET;
    lock.l_start=0;
    lock.l_len=0; 
    
    int fd= open("db",O_RDWR);
    if(fd==-1){
      perror("Error opening the file");
      return 1;
    }
    printf("Before entering into the critical section\n");
    fcntl(fd,F_SETLKW,&lock);
    printf("\nInside the critical section");
    read(fd,&db,sizeof(db));
    printf("\nticket no is:%d",db.ticket_no);
    db.ticket_no++;
   
    lseek(fd,0,SEEK_SET);
    int fd_write = write(fd,&db,sizeof(db));
    if(fd_write==-1){
      perror("Error writing to the file");
      close(fd);
      return 1;
    }
   
    printf("\npress enter to unlock");
    getchar();
   
    //unlocking it
    lock.l_type= F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    printf("\nExited critical section");
   
    close(fd);
    return 1;
}
