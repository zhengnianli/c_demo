#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *s = "hello";

    printf("sizeof(char) = %u\n", sizeof(char)); 
    printf("sizeof(char*)= %u\n", sizeof(char*)); 
    printf("sizeof('a')  = %u\n", sizeof('a')); 
    printf("sizeof(*s+0) = %u\n", sizeof(*s+0)); 
    printf("sizeof(*s)   = %u\n", sizeof(*s)); 
    printf("sizeof(s)    = %u\n", sizeof(s)); 
    system("pause");
    return 0;
}