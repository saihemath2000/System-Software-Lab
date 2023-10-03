#include <sys/ipc.h>   
#include <sys/sem.h>   
#include <sys/types.h> 
#include <sys/shm.h>   
#include <unistd.h>    
#include <stdio.h>  

void main(){

    union semun{
        int val;               // Value of the semaphore
        struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
        unsigned short *array; /* Array for GETALL, SETALL */
        struct seminfo *__buf; /* Buffer for IPC_INFO */
    } semvar;

    key_t key=ftok(".",'e');
    if(key==-1){
      perror("ftok returns");
      return;
    }

    int semid= semget(key,1,0);
    if(semid==-1){
        semid = semget(key,1,IPC_CREAT|0700);
        if(semid==-1){
          perror("semget returns");
          return;
        }

        semvar.val = 1; // Set a binary semaphore
        int semctlflag = semctl(semid,0,SETVAL,semvar);
        if(semctlflag==-1){
          perror("semctl returns");
          return;
        }
    }

    key_t shmkey = ftok(".",'f');

    if(shmkey==-1){
      perror("shm ftok returns");
      return;
    }

    int shmid= shmget(shmkey,20,IPC_CREAT|0700); // 20 is shmsize

    if(shmid==-1){
      perror("shmget returns");
      return;
    }

    char *shmpointer= shmat(shmid,(void *)0,0);

    if(*shmpointer==-1){
      perror("Error while attaching address space!");
      return;
    }

    struct sembuf semOp;
    semOp.sem_num = 0;
    semOp.sem_flg = 0;

    printf("Press enter to lock the critical section!\n");
    getchar();
    
    // Use semaphore to lock the critical section
    semOp.sem_op = -1;
    int semopflag = semop(semid,&semOp,1);
    if(semopflag==-1){
      perror("semop returns");
      return;
    }
    
    printf("Critical Section is now locked!\n");
    printf("Start of the critical section!\n");

    printf("Writing to the shared memory!\n");
    sprintf(shmpointer, "andhrapradesh");

    printf("Press enter to read from the shared memory!\n");
    getchar();
    printf("%s\n", shmpointer);

    printf("Press enter to exit the critical section!\n");
    getchar();

    // Use semaphore to unlock the critical section
    semOp.sem_op = 1;
    semopflag = semop(semid,&semOp,1);
    if(semopflag==-1){
      perror("Error while operating on semaphore!");
      return;
    }

    printf("Critical section is now unlocked!\n");
}
