/*
============================================================================
Name : 29.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to remove the message queue.
Date: 30th Sept 2023.
============================================================================
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(){
    key_t key = ftok(".",'a');
    if(key==-1){
      perror("ftok");
      return 1;
    }
    
    int mqid = msgget(key, 0);
    if(mqid==-1){
      perror("msgget");
      return 1;
    }

    if(msgctl(mqid,IPC_RMID,NULL)==-1){
      perror("msgctl IPC_RMID");
      return 1;
    }
    printf("Message queue has been removed.\n");
    return 0;
}

