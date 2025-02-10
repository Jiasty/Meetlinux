#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const int CacheSize = 1024;

std::string GetOtherMessage()
{
    static int cnt = 0;
    std::string messageid = std::to_string(cnt); // stoi -> string -> int
    cnt++;
    pid_t self_id = getpid();
    std::string stringpid = std::to_string(self_id);

    std::string message = "messageid: ";
    message += messageid;
    message += " my pid is : ";
    message += stringpid;

    return message;
}

void SubProcessSend(int wfd)
{
    std::string message = "Father, I am your child!";
    while(true)
    {
        std::cerr << "+++++++++++++++++++++++++++++++++" << std::endl;
        std::string info = message + GetOtherMessage(); //子进程将要发送的信息
        write(wfd, info.c_str(), info.size()); // 写入管道的时候，没有写入\0, 有没有必要？没有必要
        std::cout << info << std::endl;
    }

    std::cout << "child quit ..." << std::endl;
}

void ParProcessAccept(int rfd)
{
    char cache[CacheSize];
    while(true)
    {
        std::cerr << "---------------------------------" << std::endl;
        ssize_t n = read(rfd, cache, sizeof(cache) - 1);
        if(n > 0)
        {
            cache[n] = 0; // 等价于'\0'
            std::cout << cache << std::endl;
        }
        else if(n == 0)
        {
            //n == 0代表读端直接关闭，读到文件末尾。
            std::cout << "We have read the end!" << std::endl;
            break;
        }
        else
        {
            std::cerr << "read error" << std::endl;
            break;
        }
        sleep(1);
    }
}

int main()
{
    //1.创建匿名管道
    int pipefd[2];
    int n = pipe(pipefd);
    if(n != 0)
    {
        std::cerr << "errno:" << errno << " errstring:" << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "pipefd[0]" << pipefd[0] << " pipefd[1]" << pipefd[1] << std::endl;

    //2.创建子进程
    pid_t id = fork();
    if(id < 0) perror("fork error!");
    else if(id == 0)
    {
        //This is child process!
        //3.关闭不需要的文件描述符(子进程写，关闭读)
        std::cout << "Child process will send message soon!" << std::endl;
        sleep(1);
        close(pipefd[0]);
        SubProcessSend(pipefd[1]);

        //子进程完成任务关闭管道写端退出
        close(pipefd[1]);
        exit(0);
    }
    //This is father process!
    //3.关闭不需要的文件描述符(父进程读，关闭写)
    std::cout << "Father process will accept message soon!" << std::endl;
    sleep(1);
    close(pipefd[1]);
    ParProcessAccept(pipefd[0]);
    sleep(5);
    close(pipefd[0]);

    //4.父进程等待子进程
    int status = 0;
    pid_t rid = waitpid(id, &status, 0);
    if(rid > 0)
    {
        std::cout << "wait child process done, exit sig: " << (status&0x7f) << std::endl;
        std::cout << "wait child process done, exit code(ign): " << ((status>>8)&0xFF) << std::endl;
    }

    return 0;
}
