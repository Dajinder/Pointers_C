#include <stdio.h>
#include <unistd.h>
//ioconst.c
//Print some of the i/o constants defined in unistd.h

void main()
{
printf("\n%d",STDIN_FILENO);
printf("\n%d",STDOUT_FILENO);
printf("\n%d",STDERR_FILENO);
}
