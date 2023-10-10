/*
============================================================================
Name : 19.c
Author : G.Sai Hemanth Kumar
Description : 
         Create a FIFO file by
           a. mknod command
           b. mkfifo command
           c. use strace command to find out, which command (mknod or mkfifo) is better.
           d. mknod system call
           e. mkfifo library function
Date: 30th Sept 2023.
============================================================================
*/


#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(){
  
  /* a.mknod command
     mknod pipe_name p
   
     b.mkfifo command
     mkfifo pipe_name
     
     c.use strace command to find out, which command (mknod or mkfifo) is better
       Strace executes the command specified to strace and displays the execution process in sequence
       with each system call that is executed as part of the command.The shell first forks a child and 
       then the child calls execve() with the specified command and then returns the result to its parent, 
       i.e. the shell.
       
       mkfifo actually calls mknod() and this mkfifo() is just doing some extra steps on top of mknod() 
       because mkfifo() is a library function whereas mknod() is the actual system call. Therefore, mknod() 
       is always going to be faster than mkfifo().
 
  */
   
   // d.mknod system call
   int e = mknod("mknod_fifo",S_IFIFO,0);
   if(e<0) 
     perror("mknod returns");
   
   // e.mkfifo library function
   e = mkfifo("mkfifo_fifo",0744);
   if(e<0)
     perror("mkfifo returns");
      
   return 1; 

}
