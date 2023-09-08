/*
============================================================================
Name : 21.c
Author : G.Sai Hemanth Kumar 
Description : 
          Write a program, call fork and print the parent and child process id.
Date: 1st Sept, 2023.
============================================================================
*/

#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
int main(){
  if(fork())
    printf("Parent process id is:%d\n",getpid());
  else
    printf("Child process id is:%d\n",getpid());  
  return 1;
}
