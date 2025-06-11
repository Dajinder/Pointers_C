#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

void main(){
    int fd = open("C:\\UWindsor\\Sem_2\\ASP\\codes\\Unix_File_System\\new.txt", O_CREAT|O_RDWR, 0777);
    printf("%d",fd);
}

