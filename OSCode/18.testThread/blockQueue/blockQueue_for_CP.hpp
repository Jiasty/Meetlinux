#pragma once
#include <iostream>
#include <queue>
#include <pthread.h>

template <class T>
class blockQueue
{
private:
    bool isEmpty()
    {
        return _blockqueue.empty();
    }

    bool isFull()
    {
        return _blockqueue.size() == _maxCap;
    }

public:
    blockQueue(int maxCap = 5)
        : _maxCap(maxCap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_p_cond, nullptr);
        pthread_cond_init(&_c_cond, nullptr);
    }

    void Push(const T &in)
    {
        pthread_mutex_lock(&_mutex);
        while (isFull()) // 用if判断没问题吗？假设有2个消费者时和pthread_cond_wait调用出错时可能会出问题，改为while。
        {
            // 等待消费者消费。
            // 可是在临界区里面啊！！！pthread_cond_wait
            // 被调用的时候：除了让自己继续排队等待，还会自己释放传入的锁。
            // 函数返回的时候，不就还在临界区了？必须先参与锁的竞争，重新加上锁，该函数才会返回！
            pthread_cond_wait(&_p_cond, &_mutex); //两个消费者都在这等待。竞争锁失败的在锁这等！之后得到锁后从这返回，可能就已经没数据了，绕过了if判断
        }
        // 此时未满 || 被唤醒，可生产
        _blockqueue.push(in);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_c_cond); // 生产好了，唤醒消费者（空时消费者会一直阻塞，需要生产者去唤醒）。
    }

    void Pop(T *out)
    {
        //假设两个消费者，生产者一次唤醒所有消费者。
        pthread_mutex_lock(&_mutex);
        while (isEmpty())
        {
            // 进入则代表需要等待生产者生产。
            pthread_cond_wait(&_c_cond, &_mutex);
        }
        *out = _blockqueue.front();
        _blockqueue.pop();
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_p_cond); // 此时不为空了，唤醒生产者可以生产了。
        //pthread_cond_broadcast();
    }

    ~blockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_p_cond);
        pthread_cond_destroy(&_c_cond);
    }

private:
    std::queue<T> _blockqueue; // 临界资源
    int _maxCap;
    pthread_mutex_t _mutex;
    pthread_cond_t _p_cond; // 生产者条件变量
    pthread_cond_t _c_cond; // 消费者条件变量

    // 在低水位和高水位时再通知生产者和消费者，就不用每Push或Pop就唤醒一次。
    // int low_water = _max_cap/3
    // int hight_water _max_cap/3*2
};