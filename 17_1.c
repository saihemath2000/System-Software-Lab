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
