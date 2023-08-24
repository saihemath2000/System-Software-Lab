#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc , char* argv[]) {
    int fd;
    char filename[100];
    char buffer; // Read one character at a time
    ssize_t bytesRead;
    // if (argc!=2){
    //     printf("You are useless");
    //     return 0;
    // }

    // Open the file in read-only mode
    fd = open(argv[1],O_RDONLY); 
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Read and display each line from the file
    while ((bytesRead = read(fd, &buffer,1)) > 0) {
        if (buffer == '\n') {
            write(STDOUT_FILENO, "\n", 1);
        } else {
            write(STDOUT_FILENO, &buffer, 1);
        }
    }

    if (bytesRead == -1) {
        perror("Error reading file");
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);

    return 0;
}
