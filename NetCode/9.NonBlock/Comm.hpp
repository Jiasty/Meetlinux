#pragma once
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

void SetNonBlock(int fd)
{
    int fl = ::fcntl(fd, F_GETFL); //使用 F_GETFL将当前的文件描述符的属性取出来(这是一个位图).
    if(fl < 0)
    {
        std::cerr << "fcntl err" << std::endl;
        return;
    }
    ::fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}