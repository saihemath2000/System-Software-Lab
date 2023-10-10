/*
============================================================================
Name : 31b.c
Author : G.Sai Hemanth Kumar
Description : 
         Write a program to create a semaphore and initialize value to the semaphore.
           b. create a counting semaphore
Date: 1st Oct 2023.
============================================================================
*/

#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>

int main(){
    int semid; 
    struct sembuf sb; 
    union semun{
        int val;
        struct semid_ds *buf;
        ushort *array;
    } semarg;

    key_t key = ftok(".",'C');
    if(key==-1){
      perror("ftok returns");
      return 1;
    }

    semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
    if(semid==-1){
      perror("semget returns");
      return 1;
    }

    // Initialize the semaphore to a count of 5
    semarg.val = 5;
    if(semctl(semid,0,SETVAL,semarg)==-1){
      perror("semctl returns");
      return 1;
    }

    printf("Semaphore created with ID %d and initial count %d\n",semid,semarg.val);
    
    /* Decrement the semaphore count by 3
    sb.sem_num=0;
    sb.sem_op=-3;
    sb.sem_flg=0;
    if(semop(semid,&sb,1)==-1){
       perror("semop");
       return 1;
    }
    printf("Semaphore count decremented by 3\n");
    
    // Remove the semaphore when done
    if(semctl(semid,0,IPC_RMID)==-1){
       perror("semctl IPC_RMID");
       return 1;
    }*/

    return 1;
}

