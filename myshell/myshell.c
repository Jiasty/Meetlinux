#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


const char* GetUserName()
{
    const char* ret = getenv("USER");
    if(ret == NULL) return "NONE";
    return ret;
}

const char* GetHostName()
{
    const char* ret = getenv("HOSTNAME");
    if(ret == NULL) return "NONE";
    return ret;
}

int main()
{
    //1.输出命令行
    printf("Username:%s\n",GetUserName());


    //2.获取用户指令
    

    //3.分割用户指令
    

    //4.检查是否为内建命令
    

    //5.执行命令
    return 0;
}
