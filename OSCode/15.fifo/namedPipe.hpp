#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const std::string commPath = "./myfifo"; // 管道文件路径
#define DEFAULTFD -1
#define WRITER 1
#define READER 2
#define WRITE O_WRONLY // 定义打开文件的方式
#define READ O_RDONLY
#define BUFFERSIZE 4096

class namedPipe
{
private:
    bool OpenFifo(int WRorRD)
    {
        _fd = open(commPath.c_str(), WRorRD); // 打开已经存在的文件
        if (_fd < 0)
            return false;
        return true;
    }

public:
    namedPipe(const std::string path, int character)
        : _path(commPath), _id(character), _fd(DEFAULTFD)
    {
        if (_id == READER)
        {
            int n = mkfifo(_path.c_str(), 0666); // 此时将文件创建好，但没映射到地址空间
            if (n != 0)
                exit(-1);
            std::cout << "Reader create a namedpipe" << std::endl;
        }
    }

    bool OpenforWrite()
    {
        // 此时才将fd映射到地址空间。
        return OpenFifo(WRITE);
    }
    bool OpenforRead()
    {
        // 此时才将fd映射到地址空间。
        return OpenFifo(READ);
    }

    int writeData(const std::string &in) // 面向字节流
    {
        return write(_fd, in.c_str(), in.size()); // 返回写了多少字节
    }

    int readData(std::string *out)
    {
        char buffer[BUFFERSIZE];
        ssize_t n = read(_fd, buffer, sizeof(buffer));
        if (n > 0)
        {
            buffer[n] = 0; // 在读取到的数据末尾添加一个空字符 '\0'
            *out = buffer;
        }
        return n;
    }

    ~namedPipe()
    {
        if (_id == READER)
        {
            int n = unlink(_path.c_str());
            if (n != 0)
                perror("unlink err!");
            std::cout << "Reader free the pipe!" << std::endl;
        }
        if (_fd != DEFAULTFD)
            close(_fd);
    }

private:
    const std::string _path;
    int _id; // 管道创建出来可能是读或写，做区分。
    int _fd; // 记录该管道文件的fd
};