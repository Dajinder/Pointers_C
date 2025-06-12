#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

void main(){
    int fd = open("new.txt", O_RDWR);
    int n = lseek(fd, 9, SEEK_SET);
    printf("Return value of offset %d\n ", n);
    char *str = "COMP 8567";
    int n1 = write(fd, str, 5);
    printf("number of bytes written from string %d\n", n1);

    int n3 = lseek(fd, 5, SEEK_CUR);
    int n4 = write(fd, str, 7);
    printf("%d\n%d\n", n3, n4);

    int n5 = lseek(fd, 3, SEEK_END);
    int n6 = write(fd, str, 6);

    printf("%d\n", n6);
}