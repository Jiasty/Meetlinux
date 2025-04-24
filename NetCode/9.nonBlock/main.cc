#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "Comm.hpp"

int main()
{
    char buffer[1024];
    SetNonBlock(0); //改变fd为非阻塞IO
    while (true)
    {
        // std::cout << "Enter#";
        // fflush(stdout);
        ssize_t n = ::read(0, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "echo#" << buffer << std::endl;
        }
        else if (n == 0) //ctrl + d
        {
            std::cout << "read done!" << std::endl;
            break;
        }
        else
        {
            // 如果是非阻塞，底层数据没有就绪，IO接口，会以出错形式返回.
            // 所以，如何区分  底层不就绪  vs   真的出错了？ 根据errno错误码
            // 底层数据没有就绪： errno 会被设置成为 EWOULDBLOCK EAGAIN
            if (errno == EWOULDBLOCK)
            {
                std::cout << "底层数据没有就绪" << std::endl;
                std::cout << "可以做其他事" << std::endl;
                sleep(1);
                continue;
            }
            else if (errno == EINTR)
            {
                // 系统调用被信号中断时可能会返回 EINTR 错误，要继续完成剩余工作。
                continue;
            }
            else
            {
                std::cerr << "read err" << std::endl;
                break;
            }
        }
    }

    return 0;
}