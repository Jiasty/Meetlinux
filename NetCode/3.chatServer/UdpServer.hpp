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
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace log_ns;
static const int gsockfd = -1;
static const uint16_t glocalport = 8888;

using server_t = std::function<void(int, const std::string &message, InetAddr &who)>;

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR
};

class UdpServer : public nocopy // 单例
{
public:
    UdpServer(server_t func, uint16_t localport = glocalport)
        : _func(func),
          _sockfd(gsockfd),
          _localport(localport),
          _isrunning(false)
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
        // local.sin_addr.s_addr = inet_addr(_localip.c_str()); // TODO //ip是struct sockaddr_in中嵌套的一个结构体中的整型变量。
        local.sin_addr.s_addr = INADDR_ANY; // 服务器端进行任意ip绑定。INADDR_ANY就是0

        int n = ::bind(_sockfd, (struct sockaddr *)&local, sizeof(local)); // 填充进内核，和文件关联起来。
        if (n < 0)
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
        while (_isrunning)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = ::recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if (n > 0) // 读取成功
            {
                InetAddr addr(peer);
                inbuffer[n] = 0;
                LOG(DEBUG, "[%s]# %s", addr.AddrStr().c_str(), inbuffer);
                _func(_sockfd, inbuffer, addr);
                LOG(DEBUG, "return udpserver\n");
            }
            else
            {
                LOG(ERROR, "recvfrom ,  error");
            }
        }
        _isrunning = false;
    }

    ~UdpServer()
    {
        if (_sockfd > gsockfd)
            ::close(_sockfd);
    }

private:
    int _sockfd;         // 读写使用同一个sockfd，反应UDP是全双工通信。
    uint16_t _localport; // 本地端口号
    // std::string _localip; // 本机ip，网络通信时是当四字节来使用的，不是字符串。TODO
    // 服务端绑定ip，指定在哪个网络接口上接收客户端的消息。为了接收所有的服务端，一般绑定为0。
    bool _isrunning; // 服务状态

    server_t _func;
};