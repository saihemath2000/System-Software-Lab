/*
============================================================================
Name : 25.c
Author : G.Sai Hemanth Kumar 
Description : 
        Write a program to create three child processes. The parent should wait for a particular child (use waitpid system call).        
Date: 5th Sept, 2023.
============================================================================
*/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/wait.h>
int main(){
   pid_t p;
   printf("before fork\n");
   p=fork();
   
  if(p==0){
    printf("i am chlid1 pid %d\n", getpid());
    printf("my chlid1 parentid is %d\n", getppid());
  }

  else{
   pid_t q;
   printf("inside first in parent\n");
   q=fork();
   if(q==0){
     printf("i am chlid2 pid is %d\n", getpid());
     printf("i am chlid2 parent pid is %d\n", getppid());
   }
   else{
     pid_t r;
     printf("inside second in parent\n");
     r=fork();
     if(r==0){
        sleep(10);
        printf(" iam third chlid is %d\n", getpid());
        printf("i am thirdchlid parent is %d\n", getppid());
        printf("my thirdchlid is completed\n");
     }
     else{
        waitpid(r,WIFEXITED(2), WCONTINUED);
        printf("i am parent of all %d\n", getpid());
     }
  }
 }
}

