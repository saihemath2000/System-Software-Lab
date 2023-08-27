#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>

struct {
  int train_no;
  int ticket_no;
}db[3];

int i;
int main(int argc, char* argv[]){
  if(argc!=2){
    printf("missing arguments");
    return 1;
  }

  for(i=0;i<3;i++){
     db[i].train_no=i+1;
     db[i].ticket_no=0;
  }    
  int fd= open(argv[1],O_CREAT|O_RDWR,0644);
  if(fd==-1){
    perror("Error opening the file");
    return 1;
  }
  int fd_write= write(fd,db,sizeof(db));
  if(fd_write==-1){
    perror("Error writing into the file");
    close(fd);
    return 1;
  }
  
  //closing the file   
  int fd_close = close(fd);
  if(fd_close==-1){
    perror("Error closing the file");
    return 1;
  }
  return 1;
}

