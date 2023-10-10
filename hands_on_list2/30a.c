/*
============================================================================
Name : 30a.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to create a shared memory.
            a. write some data to the shared memory
Date: 1st Oct 2023.
============================================================================
*/



#include<stdio.h>
#include<sys/shm.h>
#include<sys/ipc.h>

int main(){
  int key,shmid;
  char *data;
  
  key = ftok(".", 'b');
  if(key==-1){
    perror("returns");
    return 1;
  }
  
  shmid = shmget(key, 1024, IPC_CREAT|0744);
  if(shmid==-1){
    perror("returns");
    return 1;
  }
  
  data = shmat(shmid, 0, 0);
  printf("Enter the text: ");
  scanf("%[^\n]", data);
  return 1;
}
