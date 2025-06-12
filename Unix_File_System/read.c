#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

void main(){
    int fd = open("C:\\UWindsor\\Sem_2\\ASP\\codes\\Unix_File_System\\new.txt", O_RDONLY);

    char *buff1;
    long int n;
    n = read(fd, buff1, 10);

    printf("%d\n", n);
    printf("%s\n", buff1);
}