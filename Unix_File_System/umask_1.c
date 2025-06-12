#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

void main()
{

umask(0000); //system call within a program, overrides the previously set mask (in the command line) 
// printf("%d\n", a);
int fd1=open("new_1.txt",O_CREAT|O_RDWR,0777);
printf("%d\n", fd1);
}
