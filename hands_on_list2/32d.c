#include <sys/types.h> 
#include <sys/ipc.h>   
#include <sys/sem.h>   
#include <unistd.h>    
#include <stdio.h>    

void main(){

    int semctlStatus;

    key_t key = ftok(".",3);
    if(key==-1){
      perror("ftok returns");
      return;
    }

    int semid = semget(key,1,IPC_CREAT|0777);

    if(semid==-1){
      perror("semget returns");
      return;
    }

    printf("Press enter to delete the semaphore %d!\n",semid);
    getchar();

    int semctlflag= semctl(semid,0,IPC_RMID);

    if(semctlflag==-1){
      perror("semctl returns");
      return;
    }
    printf("Semaphore deleted successfully!!");
}
