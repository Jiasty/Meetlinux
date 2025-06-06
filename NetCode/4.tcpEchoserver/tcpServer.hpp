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
#include "threadPool.hpp"

using namespace log_ns;
const static uint16_t gport = 8888;
const static int gsockfd = -1;
const static int gbacklog = 8; // 不能设置太大.

using task_t = std::function<void()>;

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR
};

class TcpServer
{
private:
    void Version_1(int sockfd, const InetAddr &addr)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // child
            ::close(_listenSockfd); // 关闭以防修改出错。

            if (fork() > 0)
                exit(0); //@将任务发给孙子进程，让其由系统托管。

            Service(sockfd, addr);
            exit(0);
        }
        // father
        ::close(sockfd);
        int n = waitpid(pid, nullptr, 0);
        if (n > 0)
        {
            LOG(INFO, "wait child success!\n");
        }
    }

    static void *Excute(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *tp = static_cast<ThreadData *>(args);
        tp->_self->Service(tp->_sockfd, tp->_addr);
        ::close(tp->_sockfd);
        delete tp;
        return nullptr;
    }

    void Version_2(int sockfd, const InetAddr &addr)
    {
        pthread_t thread;
        ThreadData *td = new ThreadData(sockfd, this, addr);
        pthread_create(&thread, nullptr, Excute, (void *)td);
    }

    void Version_3(int sockfd, const InetAddr &addr)
    {
        // ThreadPool<>* tpp = new ThreadPool<>();
        task_t t = std::bind(&TcpServer::Service, this, sockfd, addr); //@@@
        ThreadPool<task_t>::GetInstance()->Push(t);
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
    TcpServer(const uint16_t port = gport)
        : _listenSockfd(gsockfd),
          _port(port),
          _isrunning(false)
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
            // 连接成功,提供服务.
            // version-0 不靠谱版本
            // Service(sockfd, addr);

            // version-1 多进程版本
            // Version_1(sockfd, addr);

            // version-2 多线程版本(推荐)
            // Version_2(sockfd, addr);

            // version-3 线程池版本
            Version_3(sockfd, addr);
        }
        _isrunning = false;
    }

    void Service(int sockfd, InetAddr addr)
    {
        // 长服务
        while (true)
        {
            char inbuffer[1024]; // 当字符串
            ssize_t n = ::read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            if (n > 0)
            {
                inbuffer[n] = 0;
                LOG(INFO, "get message %s from %s\n", inbuffer, addr.AddrStr().c_str());
                std::string echo_string = "[server echo]#";
                echo_string += inbuffer;
                ::write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                // 读到文件结尾,客户端退出.
                LOG(INFO, "client %s quit!\n", addr.AddrStr().c_str());
                break;
            }
            else
            {
                LOG(ERROR, "read error: %s\n", addr.AddrStr().c_str());
                break;
            }
        }
        ::close(sockfd);
    }

    ~TcpServer()
    {
    }

private:
    int _listenSockfd; // 监听套接字
    uint16_t _port;
    bool _isrunning;
};