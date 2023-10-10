/*
============================================================================
Name : 26.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to send messages to the message queue. Check $ipcs -q
Date: 30th Sept 2023.
============================================================================
*/


#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>

int main() {
  
  struct msg {
    long int m_type;
    char message[80];
  }myq;
  
  key_t key = ftok(".", 'a');
  if(key==-1){
    perror("returns");
    return 1;
  }
  
  int mqid = msgget(key,IPC_CREAT|0700);
  if(mqid==-1){
    perror("returns");
    return 1;
  }
  
  printf("Enter message type:");
  scanf("%ld", &myq.m_type);
  
  printf("Enter message text:");
  scanf(" %[^\n]s", myq.message);  //maintain space before %, This space is used to consume any leading whitespace characters,  including the newline character left in the input buffer from the previous input

  int size = strlen(myq.message);
  int y = msgsnd(mqid, &myq, size + 1,0);
  if(y==-1){
    perror("error while sending data");
    return 1;
  }
  
  printf("Message sent successfully!!");
  return 1;
}

//ipcrm -Q 0x62061aab to delete messages with msgkey
