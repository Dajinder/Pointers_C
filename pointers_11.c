#include<stdio.h>
#include<stdlib.h>

void main(){
    int *ar;
    int n;

    printf("\nenter number of elements required in array\n");
    scanf("%d",&n);

    ar = malloc(n*sizeof(int));
    int i;
    for (i=0;i<n;i++){
        printf("");
    }
}
