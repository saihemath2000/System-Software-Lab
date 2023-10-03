/*
============================================================================
Name : 14.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to find the type of a file.
            a. Input should be taken from command line.
            b. program should be able to identify any type of a file.
Date: 25th Aug, 2023.
============================================================================
*/



#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<stdbool.h>

static void displaytypeinfo(const struct stat *sb){
    printf("\nFile type:   ");
    switch(sb->st_mode & S_IFMT){
      case S_IFREG:
                printf("Regular file");
                break;
      case S_IFDIR:
                printf("Directory file");
                break;
      case S_IFCHR:
      // for character and block device files go to /dev directory
                printf("Character device file");
                break;
      case S_IFBLK:
                printf("Block device file");
                break;
      case S_IFLNK:
                printf("Symbolic (soft) link file");
                break;
      case S_IFIFO:
                printf("FIFO file"); 
                break;          
      case S_IFSOCK:
                printf("Socket file");
                break;         
      default:
             printf("unknown file type");           
    }


}

int main(int argc, char* argv[]){
    if(argc!=2){
      printf("missing arguments");
      return 1; 
    }
    struct stat sb;
    bool statlink;
    
    statlink = strcmp(argv[1],"-l")==0;
    if(statlink==0){
      if(lstat(argv[1],&sb)==-1)
        perror("lstat");
    }else{
      if(stat(argv[1],&sb)==-1)
         perror("stat");
    }
    
    displaytypeinfo(&sb);    
    return 1; 
}
