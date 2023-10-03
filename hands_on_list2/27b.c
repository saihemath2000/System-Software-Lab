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
  
  int mqid = msgget(key, 0);
  if(mqid==-1){
    perror("returns");
    return 1;
  }
  
  printf("Enter message type: ");
  scanf("%ld", &myq.m_type);
  
  int ret = msgrcv(mqid, &myq, sizeof(myq.message), myq.m_type,IPC_NOWAIT|MSG_NOERROR);
  if(ret==-1){
    perror("returns");
    return 1;
  }
  
  printf("Message type: %ld\n Message: %s\n", myq.m_type, myq.message);
  return 1;
}
