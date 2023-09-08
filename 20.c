/*
============================================================================
Name : 20.c
Author : G.Sai Hemanth Kumar 
Description : 
          Find out the priority of your running program. Modify the priority using getpriority and setpriority command.
Date: 1st Sept, 2023.
============================================================================
*/


#include<sys/resource.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
  if(argc!=2){
    printf("missing arguments");
    return 1;
  }
  
  int which = PRIO_PROCESS,who=0;
  int prio = getpriority(which,who);
  int new_prio = atoi(argv[1]);
  
  if(prio==-1)
     perror("Error:");
     
  printf("Priority of process %d is %d\n",getpid(),prio);
  if(setpriority(which,who,new_prio)==-1){
    perror("Error returns:");
    return 1;
  }
  prio = getpriority(which,who);
  printf("New priority of process %d is %d\n",getpid(),prio);
 while(1);   
  return 1;
}
