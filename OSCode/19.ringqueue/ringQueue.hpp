#pragma once
#include <vector>
#include <pthread.h>
#include <semaphore.h>

template <class T>
class ringQueue
{
private:
    void P(sem_t &sem)
    {
        sem_wait(&sem);
    }

    void V(sem_t &sem)
    {
        sem_post(&sem);
    }

public:
    ringQueue(const int maxcap = 5)
        : _ringqueue(maxcap),
          _maxcap(maxcap)
    {
        sem_init(&_space_sem, 0, maxcap);
        sem_init(&_data_sem, 0, 0);
        pthread_mutex_init(&_Pmutex, nullptr);
        pthread_mutex_init(&_Cmutex, nullptr);
    }
    void Push(const T &in) // 生产
    {
        // 信号量：是一个计数器，是资源的预订机制。预订：在外部，可以不判断资源是否满足，就可以知道内部资源的情况！
        P(_space_sem); // 信号量这里，对资源进行使用，申请，为什么不判断一下条件是否满足？？？信号量本身就是判断条件！成功即满足。
        pthread_mutex_lock(&_Pmutex); //先申请信号量后申请锁，到这时直接就拥有了信号量。
        _ringqueue[_P_pos++] = in;
        _P_pos %= _maxcap;
        pthread_mutex_unlock(&_Pmutex);
        V(_data_sem);
    }
    void Pop(T *out) // 消费
    {
        P(_data_sem);
        pthread_mutex_lock(&_Cmutex);
        *out = _ringqueue[_C_pos++];
        _C_pos %= _maxcap;
        pthread_mutex_unlock(&_Cmutex);
        V(_space_sem);
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
    // 信号量保证了生产者与消费者之间的互斥关系
    sem_t _space_sem; // 生产者关心
    sem_t _data_sem;  // 消费者关心
    // 加锁是为了保证多线程时，生产者与生产者、消费者与消费者之间互斥关系。
    pthread_mutex_t _Pmutex;
    pthread_mutex_t _Cmutex;
};