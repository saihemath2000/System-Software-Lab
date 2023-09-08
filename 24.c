/*
============================================================================
Name : 24.c
Author : G.Sai Hemanth Kumar 
Description : 
        Write a program to create an orphan process.         
Date: 1st Sept, 2023.
============================================================================
*/


#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(){
   if(!fork()){
     printf("I'm child process, my parent_id before it left me:%d\n",getppid());
     //printf("My process id %d\n",getpid());
     sleep(1);
     printf("I'm child process, my current grandparent is:%d",getppid());
   }
   else{
     printf("This is parent process:%d\n",getpid());
     exit(0);
   }
}
