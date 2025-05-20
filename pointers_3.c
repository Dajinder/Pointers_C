#include<stdio.h>

void main(){
    int n1 = 10,n2; //integer varaibles n1 and n2
    int *ptr; //declaration of pointer

    ptr = &n1; // pointer stores the address of n1
    n2 = *ptr; //dereferencing of pointer; n2 stores the value of pointer

    printf("\nn1: %d, n2: %d, add of n1: %d, ptr: %d, *ptr: %d\n", n1,n2,&n1,ptr,*ptr); // n1 should be 10, n2 should be 10, add of n1 and ptr should be same, *ptr should be 10
}