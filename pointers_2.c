#include<stdio.h>
void main(){
    int *a; //pointer to a variable of integer type

    int b= 100; //int variable

    a = &b; //pointer storing the address of integer b

    printf("\nThe value of a before update %d\n", *a); // dereferncing before updation; should contain 100

    *a = 200; // derferencing; value of a is updated to 200 in memory

    printf("\nAddress of b: %d\n", &b); // print address of b
    printf("\nValue of a: %d\n", a); // print value of a (which eventually stores address stored of integer b)

    printf("\nThe value of b: %d\n", b); // print value of b
    printf("\nThe value of b: %d\n", *a); // print value of b via derefencing of a after updation
}