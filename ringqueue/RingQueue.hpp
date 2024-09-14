#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <semaphore.h>
#include <pthread.h>

template <class T>
class RingQueue
{
private:
    // PV操作
    void P(sem_t &s)
    {
    }
    void V(sem_t &s)
    {
    }

public:
    RingQueue(int maxCapacity)
        : _ringQueue(maxCapacity), _maxCapacity(maxCapacity), _cStep(0), _pStep(0)
    {
        sem_init(&_dataSem, 0, 0);
        sem_init(&_spaceSem, 0, maxCapacity);
    }

    void Push(const T &in)
    {
    }
    void Pop(T *out)
    {
    }
    ~RingQueue()
    {
        sem_destroy(&_dataSem);
        sem_destroy(&_spaceSem);
    }

private:
    std::vector<T> _ringQueue;
    int _maxCapacity;

    int _cStep;
    int _pStep;

    sem_t _dataSem;  // 消费者关心
    sem_t _spaceSem; // 生产者关心
};