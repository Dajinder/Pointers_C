#include<stdio.h>
void main(){
    int ar[4] = {5,10,15,20};
    int *ptr;
    int i;

    // Printing value using dereferencing of ar
    printf("\nusing dereferencing of ar\n");
    for (i=0;i<4;i++){
        printf("\n%d,%d\n",(ar+i),*(ar+i)); //print address of the element and value of element 
    }

    ptr = ar;

    // Printing value using dereferencing of ptr
    printf("\nusing dereferencing of ptr\n");
    for(i=0;i<4;i++){
        printf("\n%d,%d\n",(ptr+i),*(ptr+i)); //print address of the element and value of element using ptr
    }

}