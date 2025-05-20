#include<stdio.h>

// difference between pointers is feasible but addition, multiplication, division and modulation is not.

void main(){
    int ar[100], *ptr1,*ptr2;

    ptr1 = ar; // this can be written as ptr1 = &ar[0] as well
    ptr2 = &ar[60];

    printf("%d,%d",ptr1,ptr2); // should print address pointed by ptr 1 and ptr2
    
    printf("\noffset(ptr2-ptr1): %d\n", ptr2-ptr1);

    ptr1 = &ar[30];
    ptr2 = &ar[90];

    printf("\nadd of ptr2: %d, add of ptr1 %d,  addoffset(ptr2-ptr1): %d\n", ptr2, ptr1, ptr2-ptr1);
}