#pragma once
#include <iostream>
#include <string>
#include <pthread.h>

namespace ThreadMoudle
{
    class ThreadData
    {
    public:
        ThreadData(const std::string &name, pthread_mutex_t *lock)
            : _name(name), _lock(lock)
        {
        }

    public:
        std::string _name;
        pthread_mutex_t *_lock;
    };

    typedef void (*func_t)(ThreadData *td); // 定义线程函数的类型,自己调整

    class Thread
    {
    public:
        void Excute()
        {
            // 解耦合
            _isRunning = true;
            this->_task(_td);
            _isRunning = false;
        }

    public:
        Thread(const std::string &name, func_t func, ThreadData *td)
            : _name(name), _task(func), _td(td)
        {
            std::cout << "Create a thread" << _name << std::endl;
        }

        static void *threadRun(void *args) // 隐含了this指针，将其变为static
        {
            Thread *self = static_cast<Thread *>(args);
            self->Excute();
            return nullptr;
        }

        bool Start()
        {
            int n = ::pthread_create(&_tid, nullptr, threadRun, this);
            return n == 0 ? true : false;
        }

        void Stop()
        {
            if (_isRunning)
            {
                ::pthread_cancel(_tid);
                _isRunning = false;
                std::cout << _name << " stop" << std::endl;
            }
        }

        void Join()
        {
            ::pthread_join(_tid, nullptr);
            std::cout << _name << " Joined" << std::endl;
        }

        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        bool _isRunning;
        func_t _task;
        ThreadData *_td;
    };
}
