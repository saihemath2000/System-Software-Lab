#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main() {
    key_t key = ftok(".",'b');
    if(key==-1){
      perror("returns");
      return 1;
    }

    int mqid = msgget(key,0);
    if(mqid==-1) {
        perror("returns");
        return 1;
    }
    struct msqid_ds msq_info;
    if(msgctl(mqid,IPC_STAT,&msq_info)==-1){
        perror("returns");
        return 1;
    }

    msq_info.msg_perm.mode = 0666;
    if(msgctl(mqid,IPC_SET,&msq_info)==-1){
        perror("returns");
        return 1;
    }

    printf("Message queue permissions have been updated.\n");
    printf("%o\n",msq_info.msg_perm.mode);
    return 0;
}

