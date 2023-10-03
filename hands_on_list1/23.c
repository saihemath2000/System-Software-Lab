/*
============================================================================
Name : 23.c
Author : G.Sai Hemanth Kumar 
Description : 
        Write a program to create a Zombie state of the running program.          
Date: 1st Sept, 2023.
============================================================================
*/


#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){

   if(!fork()){
      printf("Child process %d is executing....\n",getpid());   
   }else{
      printf("Parent process %d is executing..\n",getpid());
      sleep(60);
      wait(0);
   }
   return 1;
}

//cat /proc/process_id/stat

