#include <memory>
#include <ctime>
#include "threadPool.hpp"
#include "Task.hpp"
#include "Log.hpp"

using namespace log_ns;

int main()
{
    EnableScreen(); //以向屏幕打印模式启动Log

    srand(time(nullptr));
    // std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>(5); // c++14智能指针
    // tp->Init();
    // tp->Start();
    int cnt = 10;
    while (cnt--)
    {
        sleep(1);
        // 持续向线程池推送任务
        Task t(rand() % 10, rand() % 10);
        ThreadPool<Task>::GetInstance()->Push(t);
        LOG(INFO, "enter a task, %s!", t.debug().c_str());
    }

    ThreadPool<Task>::GetInstance()->Stop();
    LOG(INFO, "thread pool stop!\n");
    sleep(10);
    return 0;
}