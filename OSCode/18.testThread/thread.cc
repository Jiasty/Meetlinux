#include <iostream>
#include <ctime>
#include <unistd.h>
#include <pthread.h>

int gval = 100;

void *handler(void *arg)
{
    while(1)
    {
        sleep(1);
        std::cout << "+I am new thread! pid: " << getpid() << "gval:"<< gval << "&gval:" << &gval << std::endl;
        

        // if(rand() % 5 == 0)
        // {
        //     int* p = nullptr;
        //     *p = 100; //制造错误
        // }
    }
    return nullptr;
}

int main()
{
    srand(time(nullptr));

    pthread_t tid1;
    pthread_create(&tid1, nullptr, handler, (void *)"Hello thread");

    pthread_t tid2;
    pthread_create(&tid2, nullptr, handler, (void *)"Hello thread");

    pthread_t tid3;
    pthread_create(&tid3, nullptr, handler, (void *)"Hello thread");

    //主线程
    while(1)
    {
        std::cout << "-I am main thread! pid: " << getpid() << "gval:"<< gval << "&gval:" << &gval << std::endl;
        sleep(1);
        //gval++;
    }

    return 0;
}