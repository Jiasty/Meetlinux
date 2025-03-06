#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "mythread.hpp"
#include "lockGuardian.hpp"

using namespace ThreadMoudle;

int tickets = 10000;

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER; // 全局锁

void ScrambleTickets(ThreadData *td)
{
    while (true)
    {
        // pthread_mutex_lock(&gmutex);
        LockGuard mutex(td->_lock); // lockguard自动加锁解锁
        if (tickets > 0)
        {
            usleep(1000); // 模拟抢票的时间消耗
            std::cout << td->_name << " get a ticket, tickets: " << tickets << std::endl;
            tickets--;
            // pthread_mutex_unlock(&gmutex);
        }
        else
        {
            // pthread_mutex_unlock(&gmutex); // 防止申请了锁却没释放，其他线程无法申请锁。
            break;
        }
    }
}

const int threadNum = 4;

int main()
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    std::vector<Thread> threads;
    for (int i = 0; i < threadNum; i++)
    {
        std::string name = "thread-" + std::to_string(i + 1);
        ThreadData *td = new ThreadData(name, &mutex);
        threads.emplace_back(name, ScrambleTickets, td);
    }

    for (auto &thread : threads)
    {
        thread.Start();
    }
    for (auto &thread : threads)
    {
        thread.Join();
    }

    pthread_mutex_destroy(&mutex); //局部锁销毁

    return 0;
}