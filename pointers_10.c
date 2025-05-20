#include<stdio.h>
#include<stdlib.h>

void main(){
    int *p1;
    float *p2;

    p1 = malloc(sizeof(int)); // used to dynamic memory allocation. can be used for dynamic array creation in the code
    p2 = malloc(sizeof(float));

    *p1 = 20;
    *p2 = 30.57;

    printf("\n%d\n", *p1);
    printf("\n%f\n", *p2);

    free(p1); //free previously allocated memory
    free(p2);

    // printf("\n%d\n", *p1);
    // printf("\n%f\n", *p2);

}