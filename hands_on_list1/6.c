/*
============================================================================
Name : 6.c
Author : G.Sai Hemanth Kumar 
Description : 
             Write a program to take input from STDIN and display on STDOUT. Use only read/write system calls
Date: 14th Aug, 2023.
============================================================================
*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(){
  char buffer[1024];
  ssize_t var;
  
  while((var= read(0,buffer,sizeof(buffer)))>0){
     write(1,buffer,var);
     exit(0);
  }
  if(var==-1){
    perror("Error writing to the file");
    return 1;
  }
  return 1;
}
