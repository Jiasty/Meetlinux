#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

void notice(int sig)
{
    std::cout << "catch a sig:" << sig << "father pid:" << getpid() << std::endl;

    // 如果有10个子进程同时退出呢？pending位图只会记录一次最后只会回收一次，所以得死循环回收.
    // 但是如果退5个，有5个永不退出，会发生阻塞。怎么办？非阻塞方式等待
    // signal(SIGCHLD, SIG_IGN);
    while (1)
    {
        pid_t rid = waitpid(-1, nullptr, WNOHANG); //-1等待任意子进程,子进程的pid也不需要传了
        if (rid > 0)
            std::cout << "wait success! rid: " << rid << std::endl;
        else if (rid < 0)
        {
            std::cout << "wait done!" << std::endl;
            break;
        }
        else
        {
            std::cout << "wait child success done " << std::endl;
            break;
        }
    }
}

void DoOtherThing()
{
    std::cout << "father task" << std::endl;
}

void testSIGCHLD()
{
    signal(SIGCHLD, notice);

    for (int i = 0; i < 10; i++)
    {
        pid_t id = fork();
        if (id == 0)
        {
            std::cout << "I am child, pid: " << getpid() << std::endl;
            sleep(3);
            exit(0);
        }
    }
    // father
    // 有了SIGCHLD信号以后父进程就不用询问式等待子进程，直接在信号捕捉里等待子进程。
    while (1)
    {
        DoOtherThing();
        sleep(1);
    }
    // sleep(100);
}

volatile int gflag = 0;

void changFlag(int sig)
{
    std::cout << "catch sig: " << sig << "change gflag 0->1" << std::endl;
    gflag = 1;
}

void testVolatile()
{
    signal(2, changFlag);
    while (!gflag)
        ; // 循环不写语句
    std::cout << "process quit normal!" << std::endl;
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

// 当对特定信号做处理时，默认该信号会被屏蔽。
// 当该信号处理完毕时，自动取消该信号的屏蔽。
void handler(int sig)
{
    std::cout << "catch a sig:" << sig << std::endl;
    while (1)
    {
        sigset_t pending;
        sigpending(&pending);
        printPending(pending);
        sleep(1);
        // sleep(5);
        // break;
    }
    // exit(1);
}

void testsigAction()
{
    struct sigaction act, oact;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, 3); // 同时屏蔽3号信号
    act.sa_flags = 0;           // 初始化暂时不用

    sigaction(2, &act, &oact);
}

void testFlow()
{
    signal(8, handler);
    signal(11, handler);
    int n = 10;
    n = n / 0;

    int *p = nullptr;
    *p = 100;
}

void testAlarm()
{
    int cnt = 0;
    signal(SIGALRM, handler);
    alarm(5);
    sleep(2);
    int n = alarm(0);
    std::cout << "n" << n << std::endl;
    // while(1)
    // {
    //     std::cout << cnt << std::endl;
    //     cnt++;
    // }
}

void testRaise()
{
    signal(3, handler); // 2将做参数传入handler
    while (1)
    {
        sleep(2);
        raise(3);
    }
}

int main()
{
    // 对信号的捕捉，我们只需要捕捉一次，后续一直有效
    // for(int i = 1; i <= 31; i++)
    // {
    //     signal(i, handler); //9号是不允许被捕捉的！
    // }
    // signal(2, handler); // 2将做参数传入handler

    // signal(3, handler);
    // signal(4, handler);

    // testRaise();
    // testAlarm();
    // testFlow();
    // testsigAction();
    // testVolatile();
    testSIGCHLD();

    while (1)
    {
        std::cout << "This is sig test! pid:" << getpid() << std::endl;
        sleep(1);
    }

    return 0;
}