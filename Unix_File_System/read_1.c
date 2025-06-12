#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

void main(){
    int fd = open("new.txt", O_RDWR);

    char *buff1 = "HELLO";

    long int n;
    n=write(fd,buff1,5);
    printf("\n\nThe number of bytes written were %ld\n",n);
    n=write(fd,buff1,5);
    printf("\n\nThe number of bytes written were %ld\n",n);
    n=write(fd,buff1,5);
    printf("\n\nThe number of bytes written were %ld\n",n);
    close(fd);

}