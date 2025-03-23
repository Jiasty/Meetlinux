#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>

namespace ThreadMoudle
{
    using func_t = std::function<void(const std::string &)>;

    class Thread
    {
    public:
        void Excute()
        {
            _isRunning = true;
            _task(_name);
            _isRunning = false;
        }

    public:
        Thread(const std::string &name, func_t func)
            : _name(name), _task(func)
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

        std::string Name()
        {
            return _name;
        }

    private:
        std::string _name;
        pthread_t _tid;
        bool _isRunning;
        func_t _task;
    };
}
