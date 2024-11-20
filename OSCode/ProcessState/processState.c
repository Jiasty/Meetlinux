#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>


void test_Orphan()
{

}

void test_Zombie()
{
    pid_t id = fork();
    if(id == -1) return;
    else if(id == 0)
    {
        //child process
        int cnt = 5;
        while(cnt)
        {
            printf("cnt:%d, I am a CHILD process, pid:%d\n", cnt, getpid());
            sleep(1);
            cnt--;
        }
        //子进程执行完后，父进程没有做任何事
    }
    else
    {
        //parent process
        while(1)
        {
            printf("I am a PARENT process, pid:%d\n", getpid());
            sleep(1);
        }
    }
}

void test_RandS()
{
    while(1)
    {
        printf("I am a process, pid:%d\n", getpid());
        sleep(1);
        //屏蔽掉printf，进程状态转为R+
    }
}

int main()
{

    //test_Zombie();
    //test_RandS();

    return 0;
}