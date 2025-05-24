#include <stdio.h>
int minimum(int num1, int num2)  
{   
    //checking minimum of 2 number
    if (num1<num2){
        return num1;
    }else{
        return num2;
    }
    
}
int maximum(int num1, int num2)  
{

    //checking maximum of 2 number
    if(num1>num2){
        return num1;
    }else{
        return num2;
    }
}
int product(int num1,int num2)  
{
    // doing product of 2 numbers
    return num1 * num2;
}

int compute(int(*minimum)(),int (*maximum)(), int (*product)(), int num1, int num2){
    return product(minimum(),maximum());
}

//calling all functions
int main(void)  
{
    int num1 = 10;
    int num2 = 5;

        minimum(num1,num2);
        maximum(num1,num2);
        product(num1,num2);

        return compute(minimum(num1, num2),maximum(num1, num2),product(num1, num2), num1,num2);


}
