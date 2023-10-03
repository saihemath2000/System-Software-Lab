#include<stdio.h>
#include<unistd.h>

int main(){
  long PIPE_BUF, OPEN_MAX;
  
  PIPE_BUF = pathconf(".",_PC_PIPE_BUF);
  OPEN_MAX = sysconf(_SC_OPEN_MAX);
  printf("Max no of files open within a process:%ld\n",OPEN_MAX);
  printf("Max amount of data that can be written to a pipe of fifo atomically is:%ld\n",PIPE_BUF);
}
