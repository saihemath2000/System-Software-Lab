#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

struct {
  int train_no;
  int ticket_no;
}db;

int main(){
  int input;
  printf("\nEnter train no 1,2,3:");
  scanf("%d",&input);
  
  struct flock lock;
  lock.l_type= F_WRLCK;
  lock.l_whence= SEEK_SET;
  lock.l_start= (input-1)*sizeof(db);
  lock.l_len= sizeof(db);
  lock.l_pid= getpid();
    
  //opening the file
  int fd= open("samplefile",O_RDWR);
  if(fd==-1){
    perror("Error opening the file");
    return 1;
  }
  
  lseek(fd,(input-1)*sizeof(db),SEEK_SET);

  int fd_read = read(fd,&db,sizeof(db));
  if(fd_read==-1){
    perror("Error reading the file");
    close(fd);
    return 1;
  }
  
  printf("Before entering into the critical section\n");
  fcntl(fd,F_SETLKW,&lock);
  printf("Inside the critical section\n");
  printf("Current ticket count is:%d\n",db.ticket_no);
  db.ticket_no++;
  
  lseek(fd,-1*sizeof(db),SEEK_CUR);
  int fd_write = write(fd,&db,sizeof(db));
  if(fd==-1){
    perror("Error writing to the file");
    close(fd);
    return 1;
  }
  
  //unlocking the file
  lock.l_type= F_UNLCK;
  fcntl(fd,F_SETLK,&lock);
  
  printf("press enter to book the ticket\n");
  getchar();
  getchar();
  
  printf("Booked ticket no is:%d\n",db.ticket_no);
   
  int close_fd= close(fd);
  if(close_fd==-1){
    perror("Error closing the file");
    return 1;
  } 
  return 1; 
}
