/*
============================================================================
Name : 23.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to print the maximum number of files can be opened within a process and size of a pipe (circular buffer).
Date: 30th Sept 2023.
============================================================================
*/



#include<stdio.h>
#include<unistd.h>

int main(){
  long PIPE_BUF, OPEN_MAX;
  
  PIPE_BUF = pathconf(".",_PC_PIPE_BUF);
  OPEN_MAX = sysconf(_SC_OPEN_MAX);
  printf("Max no of files open within a process:%ld\n",OPEN_MAX);
  printf("Max amount of data that can be written to a pipe of fifo atomically is:%ld\n",PIPE_BUF);
}
