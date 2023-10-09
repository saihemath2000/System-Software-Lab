# Lab Exercises

## Hands-on List 1

### File Management

1. Create the following types of a files using (i) shell command (ii) system call 
    1. Soft link (`symlink` system call) 
    2. Hard link (`link` system call) 
    3. FIFO (`mkfifo` Library Function or `mknod` system call)
2. Write a simple program to execute in an infinite loop at the background. Go to `/proc` directory and identify all the process related information in the corresponding `proc` directory.
3. Write a program to create a file and print the file descriptor value. Use `creat()` system call
4. Write a program to open an existing file with read write mode. Try `O_EXCL` flag also.
5. Write a program to create five new files with an infinite loop. Execute the program in the background and check the file descriptor table at `/proc/pid/fd`.
6. Write a program to take input from `STDIN` and display on `STDOUT`. Use only `read`/`write` system calls
7. Write a program to copy `file1` into `file2` (`$cp file1 file2`).
8. Write a program to open a file in read only mode, read line by line and display each line as it is read. Close the file when end of file is reached.
9. Write a program to print the following information about a given file. 
    1. Inode 
    2. Number of hard links 
    3. UID 
    4. GID 
    5. Size 
    6. Block size 
    7. Number of blocks 
    8. Time of last access 
    9. Time of last modification 
    10. Time of last change
10. Write a program to open a file with read write mode, write 10 bytes, move the file pointer by 10 bytes (use `lseek`) and write again 10 bytes. 
    1. Check the return value of `lseek` 
    2. Open the file with `od` and check the empty spaces in between the data.
11. Write a program to open a file, duplicate the file descriptor and append the file with both the descriptors and check whether the file is updated properly or not. 
    1. Use `dup` 
    2. Use `dup2` 
    3. Use `fcntl`
12. Write a program to find out the opening mode of a file. Use `fcntl`.
13. Write a program to wait for a `STDIN` for 10 seconds using `select`. Write a proper print statement to verify whether the data is available within 10 seconds or not (check in `$man 2 select`).
14. Write a program to find the type of a file. 
    1. Input should be taken from command line. 
    2. Program should be able to identify any type of a file.
15. Write a program to display the environmental variable of the user (use `environ`).
16. Write a program to perform mandatory locking. 
    1. Implement write lock 
    2. Implement read lock
17. Write a program to simulate online ticket reservation. Implement write lock. 
Write a program to open a file, store a ticket number and exit. 
Write a separate program, to open the file, implement write lock, read the ticket number, increment the number and print the new ticket number then close the file.
18. Write a program to perform Record locking. 
    1. Implement write lock 
    2. Implement read lock

    Create three records in a file. Whenever you access a particular record, first lock it then modify/access to avoid race condition.

### Process Management

1. Write a program to find out time taken to execute `getpid` system call. Use time stamp counter.
2. Find out the priority of your running program. Modify the priority with `nice` command.
3. Write a program, call `fork` and print the parent and child process id.
4. Write a program, open a file, call `fork`, and then write to the file by both the child as well as the parent processes. Check output of the file.
5. Write a program to create a Zombie state of the running program.
6. Write a program to create an orphan process.
7. Write a program to create three child processes. The parent should wait for a particular child (use `waitpid` system call).
8. Write a program to execute an executable program. 
    1. Use some executable program 
    2. Pass some input to an executable program. (for example execute an executable of `$./a.out name`)
9. Write a program to execute `ls -Rl` by the following system calls 
    1. `execl` 
    2. `execlp` 
    3. `execle` 
    4. `execv` 
    5. `execvp`
10. Write a program to get maximum and minimum real time priority.
11. Write a program to get the scheduling policy and modify the scheduling policy (`SCHED_FIFO`, `SCHED_RR`).
12. Write a program to run a script at a specific time using a Daemon process.

---

## Hands-on List 2

14. Write a simple program to create a pipe, write to the pipe, read from pipe and display on the monitor.
15. Write a simple program to send some data from parent to the child process.
16. Write a program to send and receive data from parent to child vice versa. Use two way communication.
17. Write a program to execute `ls -l | wc`
    1. Use `dup`
    2. Use `dup2`
    3. Use `fcntl`
18. Write a program to find out total number of directories on the `pwd`. Execute `ls -l | grep ^d | wc`. Use only `dup2`.
19. Create a FIFO file by 
    1. `mknod` command
    2. `mkfifo` command
    3. Use `strace` command to find out, which command (`mknod` or `mkfifo`) is better. 
    4. `mknod` system call 
    5. `mkfifo` library function
20. Write two programs so that both can communicate by FIFO - Use one way communication.
21. Write two programs so that both can communicate by FIFO - Use two way communications.
22. Write a program to wait for data to be written into FIFO within 10 seconds, use `select` system call with FIFO.
23. Write a program to print the maximum number of files can be opened within a process and size of a pipe (circular buffer). 
24. Write a program to create a message queue and print the key and message queue id.
25. Write a program to print a message queue's (use `msqid_ds` and `ipc_perm` structures)
    1. Access permission 
    2. `uid`, `gid` 
    3. Time of last message sent and received 
    4. Time of last change in the message queue
    5. Size of the queue 
    6. Number of messages in the queue
    7. Maximum number of bytes allowed
    8. PID of the `msgsnd` and `msgrcv`
26. Write a program to send messages to the message queue. Check `ipcs -q`
27. Write a program to receive messages from the message queue 
    1. With `0` as a flag 
    2. With `IPC_NOWAIT` as a flag
28. Write a program to change the exiting message queue permission. (Use `msqid_ds` structure)
29. Write a program to remove the message queue.
30. Write a program to create a shared memory. 
    1. Write some data to the shared memory
    2. Attach with `O_RDONLY` and check whether you are able to overwrite. 
    3. Detach the shared memory 
    4. Remove the shared memory
31. Write a program to create a semaphore and initialize value to the semaphore. 
    1. Create a binary semaphore 
    2. Create a counting semaphore
32. Write a program to implement semaphore to protect any critical section. 
    1. Rewrite the ticket number creation program using semaphore
    2. Protect shared memory from concurrent write access 
    3. Protect multiple pseudo resources (may be two) using counting semaphore 
    4. Remove the created semaphore
33. Write a program to communicate between two machines using socket.
34. Write a program to create a concurrent server. 
    1. Use `fork` 
    2. Use `pthread_create`
---------

# Mini-project

## Title: Design and Development of Course Registration Portal (Academia).

### Description

The project aims to develop a Academia Portal that is user-friendly and multifunctional.

1. All Student and Faculty details and Course information are stored in files..
2. Account holders have to pass through a login system to enter their accounts, and all these User accounts will be managed by the Administrator.
3. Roles to implement: Faculty, Student, Admin.
4. The application should possess password-protected administrative access, thus preventing the whole management system from unauthorized access
5. Once the Admin connects to the server, He/She should get a login and password prompt.
6. After successful login, you should get menu for example:
   
    Do you want to:

    - Add Student
    - Add Faculty
    - Activate/Deactivate Student 
    - Update Student/Faculty details
    - Exit
      
7. Once the Student connects to the server, He/She should get a login and password prompt.
8. After successful login, He/She should get menu for example:

    Do you want to:

     - Enroll to new Courses
     - Unenroll from already enrolled Courses
     - View enrolled Courses
     - Password Change
     - Exit
      
9. Once the Faculty connects to the server, He/She should get a login and password prompt.
10. After successful login, He/She should get menu for example:

   Do you want to:
   
   - Add new Course
   - Remove offered Course
   - View enrollments in Courses
   - Password Change
   - Exit
     
11. If you want to view the Course details then read lock is to be set else if you want to Enroll or Unenroll then write lock should be used to protect the critical data section.
12. Use socket programming – Server maintains the database and serves multiple clients concurrently. Clients can connect to the server and access their specific academic details.
13. Use System calls instead of Library functions wherever it is possible in the project: Process Management, File Management, File Locking, Semaphores, Multithreading and Inter Process   Communication Mechanisms.

