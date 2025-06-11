#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main(){
    int a;
    a = fork();
    a = fork();
    printf("\nProcess id = %d, Parent id= %d", getpid());
    return 0;
}