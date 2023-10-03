#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
   char buff1[50];
   char buff2[50]; 
   
   int fd1 = open("myfifo1", O_RDONLY);
   int fd2 = open("myfifo2", O_WRONLY);
   if(fd1==-1 || fd2==-1){
     printf("unable to open files\n");
     return 1;
   }
   
   int fd_read = read(fd1, buff1, sizeof(buff1));
   if(fd_read==-1){
     perror("returns:");
     close(fd1);
     return 1;
   }
   printf("The text from program 1 is: %s\n", buff1);
   
   printf("Enter the text:\n");
   scanf(" %[^\n]", buff2);
   
   int fd_write = write(fd2, buff2, sizeof(buff2));
   if(fd_write==-1){
     perror("returns:");
     close(fd2);
     return 1;
   }
   
   close(fd1);
   close(fd2);
   return 1;
}
