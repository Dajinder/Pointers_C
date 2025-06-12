#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

void main(){
    int fd = open("C:\\UWindsor\\Sem_2\\ASP\\codes\\Unix_File_System\\new.txt", O_CREAT|O_RDWR, 0777);
    long int n = lseek(fd, 5, SEEK_SET);
    printf("The Resulting offset is %d\n", n);

    char *buff1 = "COMP 8567";
    n = write(fd, buff1, 5);
    printf("The number of bytes from resulting offset is %d\n", n);
    close(fd);
}