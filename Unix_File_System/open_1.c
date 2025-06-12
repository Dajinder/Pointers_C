#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

void main(){
    int a = open("C:\\UWindsor\\Sem_2\\ASP\\codes\\Unix_File_System\\new.txt", O_RDONLY);
    printf("%d", a);
}