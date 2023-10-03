/*
============================================================================
Name : 17.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to execute ls -l | wc.
           a.use dup
           b.use dup2
           c.use fcntl  
Date: 30th Sept 2023.
============================================================================
*/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

void func_dup(int fd[]){
  if(!fork()){
    close(1); // close STDOUT
    close(fd[0]);
    dup(fd[1]);
    execlp("ls","ls","-l",NULL);  //execlp() will write output of "ls -l" to fd with value = 1
  }
  else{
    close(0); // close STDIN
    close(fd[1]);
    dup(fd[0]);
    execlp("wc","wc",NULL); //execlp() will read input from fd with value = 0 (read-end of pipe) as input to “wc” command 
                            //and show output to fd with value =1 => STDOUT
  }
}

void func_dup2(int fd[]){
  if(!fork()){
    close(1);
    close(fd[0]);
    dup2(fd[1],1);
    execlp("ls","ls","-l",NULL);
  }
  else{
    close(0);
    close(fd[1]);
    dup2(fd[0],0);
    execlp("wc","wc",NULL); 
  }
}

void func_fcntl(int fd[]){
  if(!fork()){
    close(1);
    close(fd[0]);
    fcntl(fd[1],F_DUPFD,1);
    execlp("ls","ls","-l",NULL);
  }
  else{
    close(0);
    close(fd[1]);
    fcntl(fd[0],F_DUPFD,0);
    execlp("wc","wc",NULL); 
  }
}

int main() { 
  int fd[2];
  pipe(fd);
  int choice;
  printf("Select one:\n");
  printf("1.dup\t2.dup2\t3.fcntl\n");
  scanf("%d",&choice);
  switch(choice){
    case 1:
        func_dup(fd);
        break;
    case 2:
        func_dup2(fd);
        break;
    case 3:
        func_fcntl(fd);
        break;
    default:
         printf("please enter correct choice\n");                
  }
}

//wc --> print newline, word, and byte counts for each FILE
