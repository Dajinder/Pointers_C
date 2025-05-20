#include<stdio.h>
void main(){
    int ar[4] = {5,10,20,25}; // name of array is a pointer pointing to address of first element i.e ar = &ar[0]

    int *ptr;
    int i;

    printf("\nUsing array\n");
    for(i=0;i<4;i++){
        printf("\n%d\n", ar[i]);
    }

    ptr = ar; //this is feasible because both ar and ptr are pointers

    printf("\nUsing pointer\n");
    for(i=0;i<4;i++){
        printf("\n%d\n",ptr[i]); //accesing array elements using ptr
    }

    // ar = ptr is not possiible because ar is already defined and continous memory is assigned to it 

}