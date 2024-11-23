#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc, char* argv[])
{
    

    //2. one programe has many functions
    if(argc > 2 || argc == 1)
    {
        printf("Usage: %s -[a/b/c/d]\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "-a") == 0)
    {
        //argv里面都是指针，不能直接与字符串比（回顾一下strcmp函数）
        printf("This is frist func, a\n");
    }
    else if(strcmp(argv[1], "-b") == 0)
    {
        printf("This is second func, b\n");
    }
    else if(strcmp(argv[1], "-c") == 0)
    {
        printf("This is third func, c\n");
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        printf("This is last func, d\n");
    }
    else
    {
        printf("fault enter!\n");
    }

    //1. view argv[]
    // for(int i = 0; i < argc; i++)
    // {
    //     printf("argv[%d]:%s\n", i, argv[i]);
    // }

    return 0;
}