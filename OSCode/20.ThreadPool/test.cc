#include <memory>
#include <ctime>
#include "threadPool.hpp"
#include "Task.hpp"
#include "Log.hpp"

int main()
{
    std::cout << GetcurTime() << std::endl;
    Log lg;
    // lg.writeMessage("test.cc", 11, DEBUG, "hello %d %s %f", 11, "std", 3.14);
    lg.writeMessage(__FILE__, __LINE__, DEBUG, "hello %d %s %f", 11, "std", 3.14);
    //__FILE__, __LINE__ 可以自动推到哪个文件哪行
    //但是这种做法还是无法直接使用，还要传替换符。可以用宏。
    lg.SwitchType(FILE);
    // lg.writeMessage("test.cc", 11, DEBUG, "hello %d %s %f", 11, "std", 3.14);
    lg.writeMessage(__FILE__, __LINE__, DEBUG, "hello %d %s %f", 11, "std", 3.14);


    // srand(time(nullptr));
    // std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>(5); //c++14智能指针
    // tp->Init();
    // tp->Start();
    // int cnt = 10;
    // while(cnt--)
    // {
    //     sleep(1);
    //     //持续向线程池推送任务
    //     Task t(rand() % 10, rand() % 10);
    //     tp->Push(t);
        
    // }

    // tp->Stop();
    // std::cout << "ThreadPool quit." << std::endl;
    // sleep(10);
    return 0;
}