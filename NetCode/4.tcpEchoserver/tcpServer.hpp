#pragma once
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace log_ns;
const static uint16_t gport = 8888;
const static int gsockfd = -1;
const static int gbacklog = 8; // 不能设置太大.

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR
};

class TcpServer
{
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
                sleep(1);
                continue;
            }
            InetAddr addr(client);
            LOG(INFO, "get a new link! cilent info: %s\n", addr.AddrStr().c_str());
            // 连接成功,提供服务.
            // version-0 不靠谱版本
            Service(sockfd, addr);
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
                // inbuffer[n] = 0;
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