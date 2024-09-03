#include <iostream>
#include <cerrno> //cc版本errno.h
#include <cstring> //cc版本string.h
#include <unistd.h>

int main()
{
    //1.创建管道
    int pipefd[2]; //做输出型参数, rfd, wfd
    int n = pipe(pipefd); //创建管道文件
    if(n != 0)
    {
        std::cerr << "cerrno" << errno << "errstring" << strerror(errno) << std::endl;
        return 1;
    }
    //pipefd[0]->0->r  pipefd[1]->1->w
    std::cout << "pipefd[0]: " << pipefd[0] << "pipefd[1]: " << pipefd[1] << std::endl; // 3  4

    //2.创建子进程
    pid_t id = fork();
    if(0 == id)
    {
        //子进程
	exit(0);
    }
    //父进程

    return 0;
}
