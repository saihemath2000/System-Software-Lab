#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main() {
    int key, shmid;
    char *data;
    
    key = ftok(".", 'b');
    if(key==-1){
        perror("key returns");
        return 1;
    }
    
    shmid = shmget(key, 1024,0);
    if(shmid==-1){
        perror("shmid returns");
        return 1;
    }
    
    if(shmctl(shmid,IPC_RMID,NULL)==-1){
      perror("shmctl returns");
      return 1;
    }

    return 0; // Return 0 to indicate success
}

