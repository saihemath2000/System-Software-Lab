/*
============================================================================
Name : 20_nice.c
Author : G.Sai Hemanth Kumar 
Description : 
          Find out the priority of your running program. Modify the priority with nice command.
Date: 1st Sept, 2023.
============================================================================
*/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s niceness_value\n", argv[0]);
        return 1;
    }
    
    int niceness = atoi(argv[1]);
    int current_niceness = nice(0);

    if(current_niceness == -1){
        perror("Error getting current niceness");
        return 1;
    }

    printf("Current niceness value: %d\n", current_niceness);
    int new_niceness = nice(niceness); 
    if (new_niceness == -1) {
        perror("Error setting new niceness");
        return 1;
    }
    printf("New niceness value: %d\n", new_niceness);
   while(1);
    return 0;
}

// renice -n prirority -p process_id can be used to change priority of running process
//nice can be used to change the priority of a new process which is not in running state

