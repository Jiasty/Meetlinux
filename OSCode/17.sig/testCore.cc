#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

struct bits
{
    uint32_t bits[400]; // 400*32
    // 40 使用除运算模运算进行定位。
    // 40/(sizeof(uint32_t)*8) = 1 -> bits[1]
    // 40%(sizeof(uint32_t)*8) = 8 -> bits[1]:8
};

int Sum(int start, int end)
{
    // sleep(100);
    int sum = 0;
    for (int i = start; i <= end; i++)
    {
        sum /= 0; // core
        sum += i;
    }
    return sum;
}

void printPending(sigset_t &pending)
{
    std::cout << "This process pending:" << std::endl;
    for (int signo = 31; signo > 0; signo--)
    {
        if (sigismember(&pending, signo))
            std::cout << 1;
        else
            std::cout << 0;
    }
    std::cout << std::endl;
}

void handler(int sig)
{
    std::cout << sig << "号信号递达" << std::endl;

    std::cout << "-------------------------------" << std::endl;
    sigset_t pending;
    sigpending(&pending);
    printPending(pending);
    std::cout << "-------------------------------" << std::endl;
}

void getPending()
{
    // 0.捕捉2号信号
    signal(2, handler);
    // signal(2, SIG_IGN); // 忽略一个信号
    // signal(2, SIG_DFL); // 信号的默认处理动作

    // 1.屏蔽2号信号
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    sigaddset(&block_set, 2); // 将需要屏蔽的信号写入位图。
    // 目前还为修改当前进程的block表

    sigprocmask(SIG_BLOCK, &block_set, &old_set);

    int cnt = 5;
    while (1)
    {
        // 2.获取pending信号集
        sigset_t ret;
        sigpending(&ret);

        // 3.打印pending
        printPending(ret);

        // 4.解除屏蔽
        if (cnt-- == 0)
        {
            std::cout << "解除屏蔽" << std::endl;
            sigprocmask(SIG_UNBLOCK, &block_set, &old_set);

            // 为何会终止进程？2号进程本来就是终止，我们先捕捉一下2号信号。
        }

        sleep(1);
    }
}

void testCore()
{
    pid_t id = fork();
    if (id == 0)
    {
        sleep(1);
        // child
        Sum(0, 100);
        exit(0);
    }
    // father
    int status = 0;
    pid_t rid = waitpid(id, &status, 0);
    if (rid == id)
    {
        printf("exit code: %d, exit sig: %d, core dump: %d\n", (status >> 8) & 0xFF, status & 0x7F, (status >> 7) & 0x1);
    }
}
int main()
{
    // sigset_t bits; //禁止用户直接设置，OS会提供信号集操作函数

    getPending();
    // testCore();

    return 0;
}