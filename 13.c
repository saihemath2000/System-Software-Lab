/*
============================================================================
Name : 13.c
Author : G.Sai Hemanth Kumar 
Description : 
           Write a program to wait for a STDIN for 10 seconds using select. Write a proper print statement to
verify whether the data is available within 10 seconds or not (check in $man 2 select).            
Date: 25th Aug, 2023.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

void main(){
   fd_set rfds;
   struct timeval tv;
   int retval;
   
   /* Watch stdin (fd 0) to see when it has input. */
   FD_ZERO(&rfds);
   FD_SET(0, &rfds);

   /* Wait up to ten seconds. */
   tv.tv_sec = 10;
   tv.tv_usec = 0;
   retval = select(1, &rfds, NULL, NULL, &tv);
           
   if (retval == -1)
       perror("select()");
   else if (retval)
       printf("Data is available now.\n");
   /* FD_ISSET(0, &rfds) will be true. */
   else
       printf("No data within ten seconds.\n");
 }
 
