#include <iostream>
#include <unistd.h>
#include "ringQueue.hpp"

void *Consumer(void *args)
{
    ringQueue<int>* rq = static_cast<ringQueue<int>*>(args);
    while(1)
    {
        //1.消费
        int ret = 0;
        rq->Pop(&ret);
        //2.处理数据
        std::cout << ret << std::endl;
        sleep(1);
    }
}

void *Productor(void *args)
{
    ringQueue<int>* rq = static_cast<ringQueue<int>*>(args);
    while(1)
    {
        //1.构造数据
        int x = rand() % 10; //也可以生产task，自己定
        int y = rand() % 10;
        
        //2.生产
        rq->Push(x + y);
        //sleep(1);
    }
}

int main()
{
    srand(time(nullptr));
    ringQueue<int> *rq = new ringQueue<int>();
    pthread_t c, p;
    pthread_create(&c, nullptr, Consumer, rq);
    pthread_create(&p, nullptr, Productor, rq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    return 0;
}