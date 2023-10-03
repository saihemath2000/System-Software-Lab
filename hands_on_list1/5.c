/*
============================================================================
Name : 5.c
Author : G.Sai Hemanth Kumar 
Description : 
             Write a program to create five new files with infinite loop. Execute the program in the background and check the  file descriptor table at /proc/pid/fd.
Date: 14th Aug, 2023.
============================================================================
*/



#include<stdio.h>
#include<fcntl.h>
int main(){
   int fd1 = creat("file1",0744);
   int fd2 = creat("file2",0744);
   int fd3 = creat("file3",0744);
   int fd4 = creat("file4",0744);
   int fd5 = creat("file5",0744); 
   while(1);
  return 1;
}
