#include<stdio.h>
void main(){
    
    int x=10,y=20;
    
    int *ptr1 = &x;
    int *ptr2 = &y;

    printf("\nx: %d, y: %d, *ptr1: %d, *ptr2: %d, &x: %d, &y: %d, ptr1: %d, ptr2: %d\n", x,y,*ptr1,*ptr2,&x,&y,ptr1,ptr2);

    ptr2 = ptr1;
    printf("\n ptr1: %d, ptr2: %d\n", ptr1,ptr2); // ptr2 will get content of ptr 1 which is eventually address of x in this case

    *ptr1 = *ptr2; // value after dereferencing ptr2 is stored in dereferecning value of ptr1
    printf("\n ptr1: %d, ptr2: %d\n", *ptr1, *ptr2); //if we ignore code in line 11 and 12 the value should be 20 for both

    *ptr2 = 4; // value of ptr2 is update to 4
    x = *ptr2; //value of x is updated by value of ptr2 i.e. 4
    printf("\n*ptr2: %d, x: %d\n", *ptr2, x);

    // ptr1 = x; //this will give type mismatch error as ptr1 should store adress and we are asssigning it the integer

}