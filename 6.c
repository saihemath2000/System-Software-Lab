#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(){
  char buffer[30];
  int var;
  var=read(0,buffer,sizeof(buffer));
  write(1,buffer,var);
  return 1;
}
