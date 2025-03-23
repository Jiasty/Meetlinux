#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <unistd.h>
#include "myThread.hpp"
#include "Log.hpp"

using namespace ThreadMoudle;
using namespace log_ns;

static const int defaultNum = 10;

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
                LOG(INFO, "%s thread sleep begin!\n", name.c_str());
                Sleep(); // 去条件变量下休眠。
                LOG(INFO, "%s thread wakeup!\n", name.c_str());
                _sleepNum--;
            }
            // 不为空 || 想退出，其实只需要判定队列为空且想退出，此时无未完成任务，且想退出，线程就退出。
            if (isEmpty() && !_isrunning) // 此时_isrunning = false, !_isrunning表示想退出，为true。
            {
                LOG(INFO, "%s thread quit\n", name.c_str());
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
        }
    }

    // 单例模式不能公有构造函数，且不能支持拷贝构造，赋值构造
    ThreadPool(int threadNum = defaultNum)
        : _threadNum(threadNum),
          _isrunning(false),
          _sleepNum(0)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
    ThreadPool(const ThreadPool<T> &tp) = delete;
    ThreadPool<T> &operator=(const ThreadPool<T> &tp) = delete;

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
            LOG(DEBUG, "construct thread %s done, init success\n", name.c_str());
        }
    }

    void Start()
    {
        _isrunning = true; // 可能也要对其加锁。
        // 统一启动线程
        for (auto &thread : _threads)
        {
            thread.Start();
            LOG(DEBUG, "start thread %s done.\n", thread.Name().c_str());
        }
    }

public:
    void Stop()
    {
        // 可能也要对其加锁。
        LockQueue();
        _isrunning = false;
        WakeupAll(); // 设为false时有可能所有线程都在休眠，可能都判断不了，所以要将他们全部唤醒。
        UnlockQueue();
        LOG(INFO, "Thread Pool Stop Success!\n");
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

    // 向外提供一个获取单例的接口，不然外部无法构造。
    // 这个函数使用了静态成员，它必须也是静态的。
    // 如果是多线程获取单例呢？
    static ThreadPool<T> *GetInstance()
    {
        if (_tp == nullptr) // 此层判断是为了创建好后，其他线程进入时不用再次加锁并判断浪费时间。
        {
            // 饿汉模式，优化程序启动时间。
            LockGuard lockguard(&_sigmutex);
            if (_tp == nullptr) // 第一次使用，创建单例
            {
                LOG(INFO, "create a threadpool\n");
                // 第一个线程执行到这被切走，多个线程进来，就会出错，要加锁。
                _tp = new ThreadPool();
                _tp->Init();
                _tp->Start();
            }
            else
            {
                LOG(INFO, "get a threadpool\n");
            }
        }

        return _tp;
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

    // 单例模式
    static ThreadPool<T> *_tp; // 为什么要用静态成员？
    // volatile static ThreadPool<T> *_tp;
    static pthread_mutex_t _sigmutex; // 单例的锁
};

template <typename T>
ThreadPool<T> *ThreadPool<T>::_tp = nullptr; // 第二个ThreadPool<T>是作用域
// volatile ThreadPool<T> *ThreadPool<T>::_tp = nullptr;
template <typename T>
pthread_mutex_t ThreadPool<T>::_sigmutex = PTHREAD_MUTEX_INITIALIZER;
