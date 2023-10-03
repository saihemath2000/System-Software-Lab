#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
   int fd1[2], fd2[2];
   pipe(fd1);
   pipe(fd2);

   //This child will only be used for writing output of "ls -Rl" to pipe 1 write-end = STDOUT
   if(!fork()){
       dup2(fd1[1], 1); 
       close(fd1[0]);
       close(fd2[0]);
       close(fd2[1]);
       execlp("ls","ls","-Rl",NULL);
   }
   else{
       // This child is responsible for running "ls -Rl | grep ^d" command where input is received from read-end of pipe 1= 
       // STDIN and output is written to write-end of pipe 2 = STDOUT
       if(!fork()){
          dup2(fd2[1], 1);
          dup2(fd1[0], 0);
          close(fd1[1]);
          close(fd2[0]);
          execlp("grep","grep","^d",NULL);
       }
   //This parent is responsible for running "ls -Rl | grep ^d | wc" command where input is received from read-end of pipe2    
   //STDIN and output is written to STDOUT
       else{
          dup2(fd2[0], 0);
          close(fd2[1]);
          close(fd1[0]);
          close(fd1[1]);
          execlp("wc","wc",NULL);
       }
   }
}
