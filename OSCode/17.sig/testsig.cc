#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
    std::cout << "catch a sig:" << sig << std::endl;
    exit(1);
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
    signal(2, handler); // 2将做参数传入handler

    // signal(3, handler);
    // signal(4, handler);

    // testRaise();
    // testAlarm();
    //testFlow();

    while (1)
    {
        std::cout << "This is sig test! pid:" << getpid() << std::endl;
        sleep(1);
    }

    return 0;
}