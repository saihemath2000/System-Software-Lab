#include<stdio.h>
#include<sys/msg.h>
#include<sys/ipc.h>

int main(){
  struct msqid_ds ds;
  
  key_t key = ftok(".", 'b'); 
  if(key==-1){
   perror("returns");
   return 1;
  }
  
  int msqid = msgget(key, IPC_CREAT|IPC_EXCL|0744);
  if(msqid==-1){
    perror("returns");
    return 1;
  } 
  
  if(msgctl(msqid, IPC_STAT, &ds)==-1){
    perror("returns:");
    return 1;
  }
  else{
    printf("Permissions:%o\n",ds.msg_perm.mode);
    printf("User id:%d\n",ds.msg_perm.uid);
    printf("Group id:%d\n",ds.msg_perm.gid);
    printf("Time of last message send:%ld\n",ds.msg_stime);
    printf("Time of last message received:%ld\n",ds.msg_rtime);
    printf("Time of last change in the message queue:%ld\n",ds.msg_ctime);
    printf("Size of the queue:%ld\n",ds.__msg_cbytes);
    printf("No of messages in the queue:%ld\n",ds.msg_qnum);
    printf("Max no of bytes allowed:%ld\n",ds.msg_qbytes);
    printf("pid of the msg send:%d\n",ds.msg_lspid);
    printf("pid of the msg receive:%d\n",ds.msg_lrpid);
  }
}


/*struct msqid_ds {
struct ipc_perm msg_perm;
  time_t msg_stime;
  time_t msg_rtime;
  time_t msg_ctime;
  unsigned long __msg_cbytes;
  msgqnum_t msg_qnum;
  msglen_t msg_qbytes;
  pid_t msg_lspid;
  pid_t  msg_lrpid;
};
*/
/* Ownership and permissions */
/* Time of last msgsnd() */
/* Time of last msgrcv() */
/* Time of last change */
/* Number of bytes in queue */
/* Number of messages in queue */
/* Maximum bytes in queue */
/* PID of last msgsnd() */
/* PID of last msgrcv() */
