/*
============================================================================
Name : 27.c
Author : G.Sai Hemanth Kumar 
Description : 
        Write a program to execute ls -Rl by the following system calls
          a.execl
          b.execlp
          c.execle
          d.execv
          e.execvp
Date: 5th Sept, 2023.
============================================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
   
    printf("Using execl:\n");
    execl("/bin/ls", "ls", "-Rl", NULL);
     
    printf("Using execlp:\n");
    execlp("ls", "ls", "-Rl", NULL);
    
    printf("Using execle:\n");
    char *envp[] = {NULL}; 
    execle("/bin/ls", "ls", "-Rl", NULL, envp);
    
    printf("Using execv:\n");
    char *temp[] ={"/bin/ls","ls","-Rl",NULL};
    execv("/bin/ls",temp);
    
    printf("Using execvp:\n");
    char *temp1[] = {"/bin/ls","ls","-Rl",NULL};
    execvp("ls",temp1);
    
    return 0;
}
