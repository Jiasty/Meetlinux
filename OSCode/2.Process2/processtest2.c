#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

void test_cwd()
{
    chdir("/home/Jiasty/OSCode"); //更改cwd
    //屏蔽时会在当前进程所处路径下创建

    FILE* fd = fopen("test.txt", "w");
    (void)fd; //取消未使用fd时的告警
    fclose(fd);

    while(1)
    {
        printf("I am a process, pid:%d\n", getpid());
        sleep(1);
    }
}

int main()
{
    test_cwd();

    return 0;
}