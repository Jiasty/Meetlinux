#include <iostream>
#include <unistd.h>
#include <pthread.h>

void *handler(void *arg)
{
    while(1)
    {
        std::cout << "+I am new thread! pid: " << getpid() << std::endl;
        sleep(1);
    }
    return nullptr;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, handler, (void *)"Hello thread");

    //主线程
    while(1)
    {
        std::cout << "-I am main thread! pid: " << getpid() << std::endl;
        sleep(1);
    }

    return 0;
}