#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>

int main(int argc, char* argv[]){
  if(argc!=2){
    printf("Missing arguments");
    return 1;
  }
  
  struct flock lock;
  
  //opening a file
  int fd = open(argv[1],O_RDWR);
  if(fd==-1){
    perror("Error in opening the file");
    return 1;
  }
  
  //setting lock attributes
  lock.l_type=F_WRLCK;
  lock.l_whence= SEEK_SET;
  lock.l_start=0;
  lock.l_len=0;
  lock.l_pid= getpid();
  
  printf("\nBefore entering into critical section");
  fcntl(fd,F_SETLKW,&lock);
  printf("\nInside the critical section");
  printf("\nPress enter to unlock");
  getchar();
  printf("\n File unlocked");
  
  //unlocking the lock
  lock.l_type=F_UNLCK;
  fcntl(fd,F_SETLK,&lock); 

  int fd_close = close(fd);
  if(fd_close==-1){
    perror("Erorr closing the file");
    return 1;
  }
  return 1;
}

