#include <iostream>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include "blockQueue_for_CP.hpp"
#include "task.hpp"

void *Consumer(void *args)
{
    blockQueue<Task> *bq = static_cast<blockQueue<Task> *>(args);
    while (1)
    {
        Task t;
        bq->Pop(&t);
        t.Excute();
        std::cout << t.result() << std::endl;
    }
}

void *Productor(void *args)
{
    srand(time(nullptr) ^ getpid());
    blockQueue<Task> *bq = static_cast<blockQueue<Task> *>(args);
    while (1)
    {
        int x = rand() % 10 + 1;
        int y = rand() % 10 + 1;
        Task t(x, y);
        bq->Push(t);
        std::cout << t.debug() << std::endl;
        sleep(1);
    }
    return nullptr;
}

int main()
{
    std::cout << 0;
    blockQueue<Task> *bq = new blockQueue<Task>();
    pthread_t c1, c2, p1, p2, p3;
    pthread_create(&c1, nullptr, Consumer, bq);
    pthread_create(&c2, nullptr, Consumer, bq);
    pthread_create(&p1, nullptr, Productor, bq);
    pthread_create(&p2, nullptr, Productor, bq);
    pthread_create(&p3, nullptr, Productor, bq);

    pthread_join(c1, nullptr);
    pthread_join(c2, nullptr);
    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);
    pthread_join(p3, nullptr);

    return 0;
}