/*
============================================================================
Name : 24.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to create a message queue and print the key and message queue id.
Date: 30th Sept 2023.
============================================================================
*/



#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<unistd.h>

int main(){

  key_t key = ftok(".", 'a'); // 'a'=ASCII value of "a" given as proj_id and "." will be string used to create key
  
  int msgid = msgget(key, IPC_CREAT|IPC_EXCL|0744);
  
  printf("key=0x%0x\t msgid=%d\n", key, msgid); // %0x for Hexadecimal value
}
