#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nocopy.hpp"
#include "lockGuardian.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace log_ns;
static const int gsockfd = -1;
static const uint16_t glocalport = 8888;

using func_t = std::function<std::string(std::string)>;

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR

};
// 一般服务器主要是用来进行网络数据读取和写入的。IO的
// 服务器IO逻辑 和 业务逻辑 解耦(回调函数)
class UdpServer : public nocopy // 单例
{
public:
    UdpServer(func_t func, uint16_t localport = glocalport)
        : _sockfd(gsockfd),
          _localport(localport),
          _isrunning(false),
          _func(func)
    {
    }
    void InitServer()
    {
        // 1.创建socket文件
        _sockfd = ::socket(AF_INET, SOCK_DGRAM, 0); // 以UDP方式打开。
        if (_sockfd < 0)
        {
            LOG(FATAL, "socket error!\n");
            exit(SOCKET_ERROR);
        }
        LOG(DEBUG, "socket create success, _sockfd: %d\n", _sockfd); // 默认3

        // 2.bind of system call
        struct sockaddr_in local;
        local.sin_family = AF_INET; //@?
        local.sin_port = htons(_localport);
        //local.sin_addr.s_addr = inet_addr(_localip.c_str()); // TODO //ip是struct sockaddr_in中嵌套的一个结构体中的整型变量。
        local.sin_addr.s_addr = INADDR_ANY; //服务器端进行任意ip绑定。INADDR_ANY就是0

        int n = ::bind(_sockfd, (struct sockaddr *)&local, sizeof(local)); //填充进内核，和文件关联起来。
        if(n < 0)
        {
            LOG(FATAL, "bind error!\n");
            exit(BIND_ERROR);
        }
        LOG(DEBUG, "socket bind success\n");
    }

    void Start()
    {
        _isrunning = true;
        char inbuffer[1024];
        while(_isrunning)
        {
            //死循环执行服务，读取客户端的消息并返回处理的结果。
            struct sockaddr_in peer; //存储发送方的信息。
            socklen_t len = sizeof(peer); //?
            ssize_t n = ::recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&peer, &len);
            if(n > 0) //读取成功
            {
                InetAddr addr(peer);
                //处理数据后发送
                inbuffer[n] = 0; //设置'\0'，形成字符串。
                //一个一个单词
                std::cout << "[" << addr.Ip() << " : "<< addr.Port() << "]say# " << inbuffer << std::endl;
                
                std::string ret = _func(inbuffer); //回调函数，还要返回
                //已经完成了翻译，但是udpserver不知道干了啥,解耦。
                ::sendto(_sockfd, ret.c_str(), ret.size(), 0, (struct sockaddr*)&peer, len);
            }
            sleep(1);
        }
    }

    ~UdpServer()
    {
        if(_sockfd > 0) ::close(_sockfd);
    }

private:
    int _sockfd;          //(网卡)文件描述符
    uint16_t _localport;  // 本地端口号
    //std::string _localip; // 本机ip，网络通信时是当四字节来使用的，不是字符串。TODO
    bool _isrunning;      // 服务状态

    func_t _func;
};