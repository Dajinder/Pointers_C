#include <stdio.h> 
#include <stdlib.h> 
int main(void) {
int *ar; //pointer to array
int n; // variable to define size of array
printf("\nEnter the number of elements in the array\n");
scanf("%d",&n); // size of arrau from user
ar=malloc(n*sizeof(int)); //dynamic allacation of arr

	for(int i=0;i<n;i++)
	{
	printf("Enter element %d\n", i);
	scanf("%d",&*(ar+i));  //user input for each array element
	} 
	printf("\nThe elements of the array are\n");
	for(int i=n;i>0;i--)
	{
	printf("\n%d",&*(ar+i)); // printing elment in reverse order
	} 
free(ar); // free aloocated memory
//End dynarr.c
}