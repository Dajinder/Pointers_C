#include<stdio.h>
void main(){

    //Arithmetic operations of pointers
    //integer is of 4 bytes so address values are increses or decreased by 4 in addition or subtraction

    int a  = 100;

    int *p = &a;
    printf("\nAddress of p: %d\n", p);
    
    p = p+1;
    printf("\nAddress of p: %d\n", p); // increase by 4 bytes
    
    p = p-1;
    printf("\nAddress of p: %d\n", p); // decreased by 4 bytes

    p = p+2;
    printf("\nAddress of p: %d\n", p); // increased by 8 bytes

    p = p-2;
    printf("\nAddress of p: %d\n", p); // decreased by 8 bytes

}