#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char* argv[]){

   if(argc!=3){
     printf("missing arguments");
     return 1;
   }
   
   //opening a file
   int fd = open(argv[1],O_RDWR|O_CREAT|O_APPEND,0644);
   if(fd==-1){
     perror("Error opening a file");
     return 1;
   }
   
   char a[sizeof(argv[1])];
   strncpy(a,argv[2],sizeof(argv[2]));
   
   // appending data into the file using our descriptor
   ssize_t bytes = write(fd,a,sizeof(a)-1);
   if(bytes==-1){
     perror("Error appending data using file desc");
     close(fd);
     return 1;
   }
   
   int new_fd1 = dup(fd);
   
   // appending data into the file using dup()
   bytes = write(new_fd1,a,sizeof(a)-1);
   if(bytes==-1){
     perror("Error appending data using dup() desc");
     close(new_fd1);
     return 1;
   }
   else{
     printf("Appended data successfully using dup() file desc");
   }
   
   int new_fd2 = dup2(fd,4);
   
   //appending data into the file using dup2()
   bytes = write(new_fd2,a,sizeof(a)-1);
   if(bytes==-1){
     perror("Error appending data using dup2() desc");
     close(new_fd2);
     return 1;
   } 
   else{
     printf("\nAppended data successfully using dup2() file desc");
   }
   
   
   int new_fd3 = fcntl(fd,F_DUPFD,5);
   
   //appending data into the file using fcntl()
   bytes = write(new_fd2,a,sizeof(a)-1);
   if(bytes==-1){
     perror("Error appending data using fcntl() ");
     close(new_fd3);
     return 1;
   } 
   else{
     printf("\nAppended data successfully using fcntl file desc");

   }
   
   int close_fd  = close(fd);
   if(close_fd==-1){
     perror("Error closing the file");
     return 1;
   }
   return 1;
}

