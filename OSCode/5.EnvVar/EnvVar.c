#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc, char* argv[], char* env[])
{
    //3.根据环境变量名获取内容getenv函数在stdlib.h头文件中
    char* tmp = getenv("PATH");
    if(NULL == tmp) return -1;
    printf("PATH:%s\n", tmp);

    //2.通过env参数获取环境变量
    for(int i = 0; env[i]; i++) //终止条件说明env表同argv表一样以NULL结束
    {
        printf("env[%d]->%s\n", i, env[i]);
    }

    //1.通过environ查看环境变量表
    // extern char** environ; //复习一下extern
    // for(int i = 0; environ[i]; i++)
    // {
    //     printf("env[%d]->%s\n", i, environ[i]);
    // }
    // return 0;
}