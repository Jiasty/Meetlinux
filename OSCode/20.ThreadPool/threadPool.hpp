#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <unistd.h>
#include "myThread.hpp"
#include "Task.hpp"

using namespace ThreadMoudle;

static const int defaultNum = 5;

template <typename T>
class ThreadPool
{
private:
    void LockQueue()
    {
        pthread_mutex_lock(&_mutex);
    }
    void UnlockQueue()
    {
        pthread_mutex_unlock(&_mutex);
    }
    void Wakeup()
    {
        pthread_cond_signal(&_cond);
    }
    void WakeupAll()
    {
        pthread_cond_broadcast(&_cond);
    }
    void Sleep()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }
    bool isEmpty()
    {
        return _taskQueue.empty();
    }

    //@@@不加static有隐含this指针，加了static函数体内部调用类内部函数就会出错。
    // 可以在初始化时bind，Handler与this已经强关联，此处看着就是没有参数。
    void Handler(const std::string &name)
    {
        // 线程的任务:从任务队列取任务。
        while (true)
        {
            LockQueue();
            while (isEmpty() && _isrunning) // while防止伪唤醒！队列为空，但是不退出才等。
            {
                _sleepNum++; // 进来在条件变量下等待肯定就休眠了，计数器加加。
                Sleep();     // 去条件变量下休眠。
                _sleepNum--;
            }
            // 不为空 || 想退出，其实只需要判定队列为空且想退出，此时无未完成任务，且想退出，线程就退出。
            if (isEmpty() && !_isrunning) //此时_isrunning = false, !_isrunning表示想退出，为true。
            {
                std::cout << name << "quie" << std::endl;
                UnlockQueue();
                break;
            }
            // 如果上面的判断_isrunning没取反，则会执行到此，此时queue中已经无数据，会出错！
            // 有任务，取任务执行
            T t = _taskQueue.front();
            _taskQueue.pop();
            UnlockQueue();
            // 取出任务，只属于自己，执行任务
            t(); // 不能在临界区执行，占用临界区，阻碍了其他线程取任务。
            std::cout << t.debug() << std::endl;
            std::cout << name << ": " << t.result() << std::endl
                      << std::endl;
        }
    }

public:
    ThreadPool(int threadNum = defaultNum)
        : _threadNum(threadNum),
          _isrunning(false),
          _sleepNum(0)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
    void Init()
    {
        // 可以调用另一个类的函数模块。
        // 将Handler与this强关联。此时func就是参数为空，返回值为void一个函数。
        func_t func = std::bind(&ThreadPool::Handler, this, std::placeholders::_1);
        // 初始化，将线程创建好
        for (int i = 0; i < _threadNum; i++)
        {
            std::string name = "Thread-" + std::to_string(i + 1);
            _threads.emplace_back(name, func); // 使用目标构造函数在内部构造，减少值拷贝。
        }
    }

    void Start()
    {
        _isrunning = true; // 可能也要对其加锁。
        // 统一启动线程
        for (auto &thread : _threads)
        {
            thread.Start();
        }
    }

    void Stop()
    {
        // 可能也要对其加锁。
        LockQueue();
        _isrunning = false; // 设为false时有可能所有线程都在休眠，可能都判断不了，所以要将他们全部唤醒。
        WakeupAll();
        UnlockQueue();
    }

    void Push(const T &in)
    {
        // 推送任务
        LockQueue();
        if (_isrunning) // 一定要运行时才可以插入任务，防止关闭threadpool时还有人在插入任务，导致线程一直退不了。
        {
            _taskQueue.push(in);
            if (_sleepNum > 0) // 只要有线程休眠就唤醒。
                Wakeup();      // 对条件变量的封装。
        }
        UnlockQueue();
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

private:
    int _threadNum;
    std::vector<Thread> _threads;
    std::queue<T> _taskQueue; // 生产消费场所,共享资源。
    bool _isrunning;

    int _sleepNum; // 记录休眠线程个数，以便后续唤醒操作判断。

    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};