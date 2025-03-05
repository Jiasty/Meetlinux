#include <iostream>
#include <unistd.h>
#include <pthread.h>

int tickets = 10000;

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;

void *ScrambleTickets(void *args)
{

    while (true)
    {
        pthread_mutex_lock(&gmutex);
        if (tickets > 0)
        {
            usleep(1000); // 模拟抢票的时间消耗
            std::cout << (char *)args << " get a ticket, tickets: " << tickets << std::endl;
            tickets--;
            pthread_mutex_unlock(&gmutex);
        }
        else
        {
            pthread_mutex_unlock(&gmutex); // 防止申请了锁却没释放，其他线程无法申请锁。
            break;
        }
    }
    return nullptr;
}

class ThreadData
{
public:
    ThreadData(std::string name, pthread_mutex_t *lock)
        : _name(name), _lock(lock)
    {
    }
    std::string _name;
    pthread_mutex_t *_lock;
};

int main()
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_t tid4;

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr); //创建局部锁
    ThreadData(" ", &lock);

    pthread_create(&tid1, nullptr, ScrambleTickets, (void *)"new1");
    pthread_create(&tid2, nullptr, ScrambleTickets, (void *)"new2");
    pthread_create(&tid3, nullptr, ScrambleTickets, (void *)"new3");
    pthread_create(&tid4, nullptr, ScrambleTickets, (void *)"new4");

    pthread_mutex_destroy(&lock);

    // 等待线程结束,让主线程阻塞等待线程的结束。
    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);
    pthread_join(tid3, nullptr);
    pthread_join(tid4, nullptr);

    return 0;
}