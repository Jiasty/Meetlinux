#pragma once
#include <vector>
#include <semaphore.h>


template<class T>
class ringQueue
{
private:
    void P()
    {

    }

    void V()
    {

    }

public:
    ringQueue(const int maxcap)
        :_ringqueue(maxcap), _maxcap(maxcap)
    {
        sem_init(&_space_sem, 0, maxcap);
        sem_init(&_data_sem, 0, 0);
    }
    void Push(const T& in) //生产
    {

    }
    void Pop(T* out) //消费
    {

    }

    ~ringQueue()
    {
        sem_destroy(&_space_sem);
        sem_destroy(&_data_sem);
    }

private:
    std::vector<T> _ringqueue;
    int _maxcap;
    int _P_pos = 0;
    int _C_pos = 0;
    sem_t _space_sem; //生产者关心
    sem_t _data_sem; //消费者关心
};