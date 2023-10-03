#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>

int main(){
    int semid; 
    struct sembuf sb; 

    key_t key = ftok("/tmp", 'S');
    if(key==-1){
      perror("ftok returns");
      return 1;
    }

    semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid==-1){
      perror("semget returns");
      return 1;
    }

    if(semctl(semid,0,SETVAL,1)==-1){
      perror("semctl returns");
      return 1;
    }

    printf("Semaphore created with ID %d\n", semid);

   /* Remove the semaphore when done
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl returns");
        return 1;
    }
   */
    return 1;
}

