#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"

// void doWork(int rfd)
// {
//     while (true)
//     {
//         int command = 0; // 存放父进程发送过来的任务码
//         int n = read(rfd, &command, sizeof(command));
//         if (n == 0)
//         {
//             std::cout << "child process:" << getpid() << " quit!" << std::endl;
//             break;
//         }
//         else if (n == sizeof(int)) // read读到多少字节n就为多少
//         {
//             std::cout << "pid is : " << getpid() << " handler task" << std::endl;
//             ExcuteTask(command);
//         }
//         sleep(1);
//     }
// }

class Channel
{
public:
    Channel(std::string &name, int wfd, pid_t id)
        : ChannelName(name), _wfd(wfd), _slaverPid(id)
    {
        // std::cout << "Create a channel" << this->ChannelName << std::endl;
    }

    std::string GetChannelName() { return ChannelName; }
    int Getwfd() { return _wfd; }
    pid_t GetslaverPid() { return _slaverPid; }
    void CloseChannel() { close(_wfd); }
    void WaitChild()
    {
        pid_t rid = waitpid(_slaverPid, nullptr, 0);
        if (rid > 0)
            std::cout << "Wait child:" << rid << " success!" << std::endl;
    }

    ~Channel() {}

private:
    std::string ChannelName;
    int _wfd;
    pid_t _slaverPid;
};

// const &: 输入型参数
// & : 输入输出型参数
// * : 输出型参数
// task_t task: 回调函数，解耦！！！？？？以后任务和进程池的代码，无关，改动任务就不用改进程池的代码，只需要在传参时改参数就行。
void CreateChannelandProcess(int num, std::vector<Channel> *channels, task_t task)
{
    // BUG?????!!!!!
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
            if (!channels->empty())
            {
                for (auto &channel : *channels)
                    channel.CloseChannel();
            }
            close(pipefd[1]);
            dup2(pipefd[0], 0); // 以后从标准输入里去读，解耦.一个公知的不用的fd！
            task();
            close(pipefd[0]);
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

int ChooseChannel(int TotalChannelNum)
{
    static int next = 0; // 记录下一次使用时使用哪个信道。
    int cur = next;
    next++;
    next %= TotalChannelNum; // 循环使用 -> 轮询
    return cur;
}

void SendTask(Channel &channel, int taskCommand) // 为何Channel &channel前加const会报错？
{
    write(channel.Getwfd(), &taskCommand, sizeof(taskCommand));
}

void ChnnelctlProcessHelper(std::vector<Channel> &channels)
{
    // 管道无法发送函数等，我们所发送的任务就是函数指针表！
    // a.选择一个任务
    int taskCommand = SelectTask();
    // b.选择一个信道和子进程
    int channelIndex = ChooseChannel(channels.size());
    // c.发送任务
    SendTask(channels[channelIndex], taskCommand);

    std::cout << "taskCommand:" << taskCommand
              << " Channel:" << channels[channelIndex].GetChannelName()
              << " childProcess:" << channels[channelIndex].GetslaverPid() << std::endl;

    sleep(1);
}
void ChnnelctlProcess(std::vector<Channel> &channels, int times = -1)
{
    if (times >= 0) // 控制循环次数
    {
        while (times--)
            ChnnelctlProcessHelper(channels);
    }
    else
    {
        while (true)
            ChnnelctlProcessHelper(channels);
    }
}

void ReclaimChannelsandProcess(std::vector<Channel> &channels)
{
    // int num = channels.size() -1;
    // while(num >= 0)
    // {
    //     channels[num].CloseChannel();
    //     channels[num--].Wait();
    // }

    // a.关闭所有channel写端，子进程会读到0，子进程全部退出。
    for (auto &channel : channels)
    {
        channel.CloseChannel();
        channel.WaitChild();
    }
    // 注意！！！写端关闭后，子进程会退出，不回收就会变成僵尸进程，所以此处一定要回收。
    // b.回收所有子进程
    // 为何要写两个循环呢？与前面创建子进程时的BUG有关！！！！
    // for (auto &channel : channels)
    // {
    //     channel.WaitChild();
    // }
}

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
    CreateChannelandProcess(processNum, &channels, doWork);
    // CreateChannelandProcess(..., doWork1);
    // CreateChannelandProcess(..., doWork2);

    // 2.通过channel控制子进程，发送并执行任务
    ChnnelctlProcess(channels, 5);

    // 3.回收子进程和管道
    ReclaimChannelsandProcess(channels);

    // sleep(100);
    return 0;
}