#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

void main(){
    int fd = open("new.txt", O_RDWR);
    char *buff1 = "Hello";

    int n = write(fd, buff1,5);
    printf("%d\n", n);

    char *buff2 = "World";
    int n1 = write(fd, buff2,3);
    printf("%d\n", n1);
}