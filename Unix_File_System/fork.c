#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>

void Main(){

int pid1,pid2,pid3,pid4;

pid1=fork();

pid2=fork();

pid3=fork();

pid4=fork();

if(pid1==0||pid2==0||pid3==0||pid4==0){

exit(0);

}

else

{

for(;;);

} }