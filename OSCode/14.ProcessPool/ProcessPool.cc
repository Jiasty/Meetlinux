#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"

void doWork(int rfd)
{

}

// const &: 输入型参数
// & : 输入输出型参数
// * : 输出型参数
void CreateChannelandProcess(int num, std::vector<Channel> *channels)
{
    // BUG?
    for (int i = 0; i < num; i++)
    {
        // 创建管道与子进程
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        if (n < 0)
            exit(1);

        pid_t id = fork();
        if (id == 0)
        {
            // child
            close(pipefd[1]);
            doWork(pipefd[0]);
            exit(0); // 子进程一定要退出，不然也会继续执行循环创建它的子进程。
        }
        // parent

        // 命名并插入channels
        close(pipefd[0]);
        std::string name = "Channel_" + std::to_string(i);
        channels->push_back(Channel(name, pipefd[1], id));
    }

    // for(auto& channel : *channels) channel.GetInfo();
}

void ChnnelctlProcess()
{}

void SendTask()
{
}

class Channel
{
public:
    Channel(std::string &name, int wfd, pid_t id)
        : ChannelName(name), _wfd(wfd), _slaverPid(id)
    {
        std::cout << "Create a channel" << this->ChannelName << std::endl;
    }

    std::string GetChannelName() { return ChannelName; }
    int Getwfd() { return _wfd; }
    pid_t GetslaverPid() { return _slaverPid; }
    void CloseChannel() { close(_wfd); }
    void WaitChild()
    {
        pid_t rid = waitpid(_slaverPid, nullptr, 0);
        if(rid > 0) std::cout << "Wait child:" << rid << "successfully!" << std::endl;
    }
    ~Channel() {}

private:
    std::string ChannelName;
    int _wfd;
    pid_t _slaverPid;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:" << argv[0] << " + ProcessNum" << std::endl;
        return 1;
    }
    LoadTasks();

    // 1.创建一批子进程和管道
    int processNum = std::stoi(argv[1]);
    std::vector<Channel> channels;
    CreateChannelandProcess(processNum, &channels);

    // 2.通过channel控制子进程，发送并执行任务
    // 管道无法发送函数等，我们所发送的任务就是函数指针表！

    // 3.回收子进程和管道

    // sleep(100);
    return 0;
}