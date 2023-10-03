#include<stdio.h>
#include<sys/shm.h>
#include<sys/ipc.h>

int main(){
  int key,shmid;
  char *data;
  
  key = ftok(".", 'b');
  if(key==-1){
    perror("returns");
    return 1;
  }
  
  shmid = shmget(key, 1024, IPC_CREAT|0744);
  if(shmid==-1){
    perror("returns");
    return 1;
  }
  
  data = shmat(shmid, 0, SHM_RDONLY);       //segmentation fault core dumped
  printf("Enter the text: ");
  scanf("%[^\n]", data);
  return 1;
}
