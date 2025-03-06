#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gcond = PTHREAD_COND_INITIALIZER; // 全局条件变量

const int threadNum = 5;

void *Wait(void *args)
{
    char *name = static_cast<char *>(args);

    while (true)
    {
        pthread_mutex_lock(&gmutex);
        pthread_cond_wait(&gcond, &gmutex /*?*/);       // 线程在这等待，阻塞.生产者线程怎么申请锁呢？
        std::cout << name << " is waked " << std::endl; // 向显示器文件打，显示器文件是共享资源，加锁防止打印干扰。
        pthread_mutex_unlock(&gmutex);
    }
}

int main()
{
    pthread_t threads[threadNum];
    for (int i = 0; i < threadNum; i++)
    {
        char *name = new char[128];
        snprintf(name, 128, "thread-%d", i + 1);
        pthread_create(&threads[i], nullptr, Wait, (void *)name);
        usleep(10000);
    }
    
    while (true)
    {
        std::cout << "开始唤醒" << std::endl;
        // 主线程唤醒
        // pthread_cond_signal(&gcond); //唤醒一个
        pthread_cond_broadcast(&gcond); // 唤醒全部
        sleep(1);
    }

    for (int i = 0; i < threadNum; i++)
    {
        pthread_join(threads[i], nullptr);
    }

    return 0;
}