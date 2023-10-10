/*
============================================================================
Name : 30c.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to create a shared memory.
           c. detach the shared memory
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
    perror("key returns");
    return 1;
  }
  
  shmid = shmget(key, 1024,0);
  if(shmid==-1){
    perror("shmid returns");
    return 1;
  }
  
  data = shmat(shmid, 0, 0);
  int k= shmdt(data);
  if(k==-1){
    perror("data returns");
    return 1;
  }
  return 1;
}
