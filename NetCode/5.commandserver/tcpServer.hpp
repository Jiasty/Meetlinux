#pragma once
#include <iostream>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace log_ns;
const static uint16_t gport = 8888;
const static int gsockfd = -1;
const static int gbacklog = 8; // 等待连接队列的最大长度,不能设置太大.

using command_service_t = std::function<void(int sockfd, InetAddr addr)>;

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR
};

class TcpServer
{
private:
    static void *Execute(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *tp = static_cast<ThreadData *>(args);
        tp->_self->_func(tp->_sockfd, tp->_addr); //回调函数
        ::close(tp->_sockfd);
        delete tp;
        return nullptr;
    }

    void Version_2(int sockfd, const InetAddr &addr)
    {
        pthread_t thread;
        ThreadData *td = new ThreadData(sockfd, this, addr);
        pthread_create(&thread, nullptr, Execute, (void *)td);
    }

    class ThreadData
    {
    public:
        int _sockfd;
        TcpServer *_self;
        InetAddr _addr;

    public:
        ThreadData(int sockfd, TcpServer *self, const InetAddr &addr)
            : _sockfd(sockfd),
              _self(self),
              _addr(addr)
        {
        }
    };

public:
    TcpServer(command_service_t func, const uint16_t port = gport)
        : _listenSockfd(gsockfd),
          _port(port),
          _isrunning(false),
          _func(func)
    {
    }

    void Init()
    {
        _listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listenSockfd < 0)
        {
            LOG(FATAL, "create socket errror!\n");
            exit(SOCKET_ERROR);
        }
        LOG(INFO, "create socket success, sockfd: %d\n", _listenSockfd);

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY; // 绑定任意ip,接收任意ip发来的数据.
        if (::bind(_listenSockfd, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            LOG(FATAL, "bind error!\n");
            exit(BIND_ERROR);
        }
        LOG(INFO, "bind success!\n");

        // tcp是面向连接的,tcp要持续能获取连接,随时监听.
        if (::listen(_listenSockfd, gbacklog) < 0)
        {
            LOG(FATAL, "listen error!\n");
            exit(LISTEN_ERROR);
        }
        LOG(INFO, "listen success!\n");
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            // 获取新连接
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sockfd = ::accept(_listenSockfd, (struct sockaddr *)&client, &len);
            if (sockfd < 0)
            {
                LOG(WARNING, "accept error! errno: %d\n", errno);
                continue;
            }
            InetAddr addr(client);
            LOG(INFO, "get a new link! cilent info: %s, sockfd: %d\n", addr.AddrStr().c_str(), sockfd);

            // version-2 多线程版本(推荐)
            Version_2(sockfd, addr);
        }
        _isrunning = false;
    }

    // 此时的任务处理在Command中
    // void Service(int sockfd, InetAddr addr)
    // {
    // }

    ~TcpServer()
    {
    }

private:
    int _listenSockfd; // 监听套接字
    uint16_t _port;
    bool _isrunning;
    command_service_t _func;
};