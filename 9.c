#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/stat.h>
#include <time.h>

static void displaytypeinfo(const char *filename){
    struct stat fileInfo; 
    if(stat(filename, &fileInfo) == -1) {
       perror("Error getting file information");
       return;
    }
    printf("File: %s\n", filename);
    printf("Inode number: %ld\n", (long)fileInfo.st_ino);
    printf("Number of hard links: %ld\n", (long)fileInfo.st_nlink);
    printf("UID: %u\n", fileInfo.st_uid);
    printf("GID: %u\n", fileInfo.st_gid);
    printf("Size: %ld bytes\n", (long)fileInfo.st_size);
    printf("Block size: %ld bytes\n", (long)fileInfo.st_blksize);
    printf("Number of blocks: %ld\n", (long)fileInfo.st_blocks);
    printf("Time of last access: %s", ctime(&fileInfo.st_atime));
    printf("Time of last modification: %s", ctime(&fileInfo.st_mtime));
    printf("Time of last change: %s", ctime(&fileInfo.st_ctime));
}

int main(int argc, char* argv[]){
    if(argc!=2){
      printf("missing arguments");
      return 1; 
    }
    displaytypeinfo(argv[1]);    
    return 1; 
}
