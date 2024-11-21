#include<stdio.h>

int main(int argc, char* argv[])
{
    

    //2. one programe has many functions

    //1. view argv[]
    for(int i = 0; i < argc; i++)
    {
        printf("argv[%d]:%s\n", i, argv[i]);
    }

    return 0;
}