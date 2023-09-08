/*
============================================================================
Name : 15.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to display the environmental variable of the user (use environ).
Date: 25th Aug, 2023.
============================================================================
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern char **environ;
int prefixMatch(const char *pre, const char *str){
    return strncmp(pre,str,strlen(pre))==0;
}
int main(){
    char **env = environ;
    while (*env!=NULL){
        if(prefixMatch("USER",*env)){
            printf("%s\n",*env);
        }
        env++;
    }
    return 0;
}
