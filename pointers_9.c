#include<stdio.h>
void main(){
    void *ptr;

    int a= 100;

    ptr = &a;

    printf("\n%d\n", ptr);

    float b = 12.45;
    ptr = &b;
    printf("\n%d\n", ptr);

    // printf("%f",*ptr); // dereferencing of void pointer is not allowed. this will throw error

    // to achieve this we need to use other pointer

    float *temp;
    temp = ptr;
    printf("%f", *temp);
    
}