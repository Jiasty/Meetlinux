#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


int main()
{
    printf("I am PARENT process, pid:%d, ppid:%d\n", getpid(), getppid());

    pid_t id = fork();
    if(id < 0) return -1;
    else if(id == 0)
    {
        //CHILD
        printf("I am CHILD process, pid:%d, ppid:%d\n", getpid(), getppid());
        //DoOtherThing();
        printf("child quit...\n");
        exit(111);
    }
    sleep(5); //休眠一下看到僵尸状态
    //PARENT
    //执行到父进程时，子进程一定是执行完了，但父进程没有对子进程做任何处理,子进程进入僵尸状态

    //父进程等待任意一个子进程
    //pid_t cid = wait(NULL);
    //pid_t cid = waitpid(-1, NULL, 0); //waitpid平替wait

    //如果想获取退出信息
    int status = 0;
    pid_t cid = waitpid(id, &status, 0);
    
    if(cid > 0) printf("wait successfully! status:%d\n", status);
    else printf("wait faily!\n");

    sleep(3);
    //利用位操作，将退出信息提取出来！！！
    printf("parent quit, status: %d, child quit code : %d, child quit signal: %d\n", status, (status>>8)&0xFF, status & 0x7F);
    //利用宏函数提取（面向不清楚status构造的人）
    if(WIFEXITED(status)) printf("parent quit, status: %d, child quit code : %d\n", status, WEXITSTATUS(status));
    return 0;
}