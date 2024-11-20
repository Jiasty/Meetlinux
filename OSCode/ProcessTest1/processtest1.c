#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>


void TaskOfChildProcess()
{
    while(1)
    {
        printf("id:%d, I am CHILD process, pid:%d, ppid:%d\n", getpid(), getppid());
        sleep(1);
    }
}
void createSomeProcess()
{
    const int childProcessNum = 5;

    for(int i = 0; i < childProcessNum; i++)
    {
        pid_t id = fork();
        if(id == 0) //父进程不会进入，则返回继续循环，再创建子进程
        {
            TaskOfChildProcess(); //如果子进程此处执行完，是会继续执行下面的代码的
        }
        sleep(1);
    }

    while(1)
    {
        printf("id:%d, I am PARENT process, pid:%d, ppid:%d\n", getpid(), getppid());
        sleep(1);
    }
}

void test_childProcess()
{
    printf("This process is start, just me! pid:%d\n", getpid());
    sleep(3);

    pid_t id = fork();
    if(id == -1) return;
    else if(id == 0)
    {
        //child process
        while(1)
        {
            printf("id:%d, I am CHILD process, pid:%d, ppid:%d\n", id, getpid(), getppid());
            sleep(1);
        }
    }
    else
    {
        //parent process
        while(1)
        {
            printf("id:%d, I am PARENT process, pid:%d, ppid:%d\n", id, getpid(), getppid());
            sleep(1);
        }
    }
}

void test_getpid()
{
    pid_t id = getpid();

    while(1)
    {
        printf("This is a process, pid:%d\n", id);
        sleep(1);
    }
}

int main()
{
    createSomeProcess();
    //test_childprocess();
    //test_getpid();

    return 0;
}
