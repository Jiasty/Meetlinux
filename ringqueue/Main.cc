#include "RingQueue.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime> //构造随机数使用

const int maxnum = 5;

void *Consumer(void *args)
{
    return nullptr;
}

void *Productor(void *args)
{
    return nullptr;
}

int main()
{
    pthread_t c, p;
    RingQueue<int> *rq = new RingQueue<int>(maxnum);
    pthread_create(&c, nullptr, Consumer, rq);
    pthread_create(&p, nullptr, Productor, rq);
    return 0;
}