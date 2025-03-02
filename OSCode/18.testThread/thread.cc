#include <iostream>
#include <ctime>
#include <vector>
#include <thread>
#include <unistd.h>
#include <pthread.h>

const int threadNum = 10;

// 可以传类之后，就可以传多个参数甚至方法了。
class ThreadData
{
public:
    ThreadData(std::string name, std::string num, int n1, int n2)
        : _name(name), _num(num), x(n1), y(n2)
    {
    }

    int Excute()
    {
        return x + y;
    }

    std::string GetName()
    {
        return _name;
    }

    std::string GetNum()
    {
        return _num;
    }

private:
    std::string _name;
    std::string _num;
    int x;
    int y;
};

class ThreadRet
{
public:
    std::string print()
    {
        return std::to_string(result);
    }

public:
    int x;
    int y;
    int result;
};

std::string PrintThreadID(pthread_t &tid)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "0x%lx", tid);
    return buffer;
}

void threadrunCpp(std::string name, int num)
{
    while(num)
    {
        std::cout << name << " num : " << num<< std::endl;
        num--;
        sleep(1);
    }   
}

void testCppThread()
{
    std::string name = "thread-1";
    std::thread mythread(threadrunCpp, std::move(name), 10);
    while(true)
    {
        std::cout << "main thhread..." << std::endl;
        sleep(1);
    }
    mythread.join();
}

// 如何正确全面看待函数返回：线程在退出的时候只需要考虑正确的返回
void *pthreadRun(void *arg)
{
    int cnt = 5;
    // char *name = (char *)arg;
    // int name = *(int *)arg;
    ThreadData *td = static_cast<ThreadData *>(arg); // 安全类别强转
    ThreadRet *ret = new ThreadRet();

    while (cnt--)
    {
        sleep(1);
        std::cout << td->GetName() << " num is:" << td->GetNum() << " cnt:" << cnt << std::endl;
        ret->result = td->Excute();
        // int m = 1 / 0; //故意出错，整个进程都崩溃。
    }
    // 不仅仅可以返回数字，也可以返回类对象。
    return (void *)ret;
}

void *threadrun(void *arg)
{
    pthread_detach(pthread_self()); // 线程将自己分离
    char *name = (char *)arg;
    while (1)
    {
        std::cout << name << "is running..." << std::endl;
        sleep(1);
        // break;
    }

    // exit(0); //直接将进程退出
    // return arg;
    pthread_exit(arg); // 等同于return arg;
}

void testMultithread()
{
    std::vector<pthread_t> tids;

    for (int i = 0; i < threadNum; i++)
    {
        char *name = new char[128];              // 堆上数据线程之间不会互相干扰。
        snprintf(name, 128, "thread-%d", i + 1); // 堆上空间为何不能sizeof(name)？
        pthread_t tid;
        pthread_create(&tid, nullptr, threadrun, (void *)name);

        tids.emplace_back(tid);
    }

    // sleep(5);
    // for(auto tid : tids)
    // {
    //     //主线程主动分离
    //     pthread_detach(tid);
    // }

    for (auto tid : tids)
    {
        pthread_cancel(tid);
        std::cout << "cancel" << PrintThreadID(tid) << std::endl;

        void *ret = nullptr; // 线程被取消线程的退出结果是：-1
        int n = pthread_join(tid, &ret);
        // std::cout << (const char *)name << "quit..." << std::endl;
        std::cout << (long long int)ret << " quit... " << n << std::endl;
        // delete (char *)name;
    }

    // sleep(100);
}

void test2()
{
    pthread_t tid;
    // pthreadRun函数传参有其他方式吗？可以传任意类型，也能传类对象地址。
    // int x = 100; // 主线程的参数也能传入新线程。

    // ThreadData td("thread1", "100"); //新线程访问了主线程栈上的变量！不推荐在栈上创建！应该向堆申请。
    ThreadData *td = new ThreadData("thread1", "100", 10, 20);
    int n = pthread_create(&tid, nullptr, pthreadRun, td); // 为何不用强转？
    if (n != 0)
    {
        std::cerr << "pthread create cerr!" << std::endl;
        exit(1);
    }

    // tid其实就是一个虚拟地址，为什么？
    std::string tid_str = PrintThreadID(tid);
    std::cout << "tid:" << tid_str << std::endl;

    // 我们期望main thread最后退出，如何保证？join
    // 如果不join呢？会造成类似僵尸进程的问题。
    std::cout << "main thread wait start..." << std::endl;
    ThreadRet *ret = nullptr;
    n = pthread_join(tid, (void **)&ret); // 阻塞等待退出。
    if (n == 0)
    {
        std::cout << "main thread wait success!" << std::endl;
        std::cout << ret->print() << std::endl;
    }
    // sleep(100);
}

int gval = 100;

void *handler(void *arg)
{
    while (1)
    {
        sleep(1);
        std::cout << "+I am new thread! pid: " << getpid() << "gval:" << gval << "&gval:" << &gval << std::endl;

        // if(rand() % 5 == 0)
        // {
        //     int* p = nullptr;
        //     *p = 100; //制造错误
        // }
    }
    return nullptr;
}
void test1()
{
    srand(time(nullptr));

    pthread_t tid1; // unsigned long int
    pthread_create(&tid1, nullptr, handler, (void *)"Hello thread");

    pthread_t tid2;
    pthread_create(&tid2, nullptr, handler, (void *)"Hello thread");

    pthread_t tid3;
    pthread_create(&tid3, nullptr, handler, (void *)"Hello thread");

    // 主线程
    while (1)
    {
        std::cout << "-I am main thread! pid: " << getpid() << "gval:" << gval << "&gval:" << &gval << std::endl;
        sleep(1);
        // gval++;
    }
}

int main()
{
    testCppThread();
    // testMultithread();
    // test2();
    // test1();

    return 0;
}