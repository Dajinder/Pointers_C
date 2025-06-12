#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
void main(){
    int fd = open("new.txt", O_RDWR);
    char *buff1 = "********************";

    long int n = write(fd, buff1, 20);

    printf("%d\n", n);
    close(fd);
}