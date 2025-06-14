#include<stdio.h>
int main(){
    int num = 100;

    int *b; // declaration of a variable(pointer) which has ability to store address of annother variable of integer or pointer

    b = &num; //instantiation of variable b which contains address of integer variable

    int **c;
    c = &b; //address of (adress of an integer variable)

    int ***d;
    d = &c; //address of address of (address of an integer variable)

    printf("\nValue of *b is: %d\n", *b); //dereferencing of pointer; this should return the value(100) at the address pointing to variable b
    printf("\nValue of *c is %d\n",*c); // return the address
    printf("\nValue of **c is: %d\n", **c); //return the value (100)
    printf("\nValue of *d is: %d\n", *d); //return the address
    printf("\nValue of **d is: %d\n", **d); // return the address
    printf("\nValue of ***d is: %d\n", ***d); // return the val (100)

    printf("\nn%d\n", &num);
    
}