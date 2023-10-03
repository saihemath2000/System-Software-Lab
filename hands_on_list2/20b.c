#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
    char buff[50];
    
    int fd = open("myfifo", O_RDONLY);
    if(fd==-1){
      printf("error opening pipe");
      return 1;
    }
    
    int fd_read = read(fd, buff, sizeof(buff));
    if(fd_read==-1){
      perror("returns:");
      close(fd);
      return 1;
    }
    printf("%s\n",buff);
    close(fd);  
  return 1;  
}
