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
#include <memory>

#include "Log.hpp"
#include "InetAddr.hpp"

namespace socket_ns
{
    using namespace log_ns;
    const static int gbacklog = 8; // 等待连接队列的最大长度

    class Socket;
    using SockPtr = std::shared_ptr<Socket>; // 为何这里用Socket不同Tcpsocket？
    // 基类指针或引用可以指向派生类对象。
    // 通过基类指针或者引用调用虚函数时，实际调用的是派生类中重写的函数，这就是运行时多态。使用 SockPtr 可以在运行时根据实际指向的对象类型来调用相应的函数。
    // 使用 SockPtr 作为返回类型和参数类型，能够为不同类型的套接字（例如 TcpSocket、后续可能添加的 UdpSocket 等）提供统一的接口。
    enum
    {
        SOCKET_ERROR = 1,
        BIND_ERROR,
        LISTEN_ERROR
    };

    // 模板方法模式@@@？操作都是一致的。
    // 以后使用套接字都采用面向对象式创建管理及使用。
    class Socket
    {
    public:
        virtual void CreateSocketOrDie() = 0;
        virtual void CreateBindOrDie(uint16_t port) = 0;
        virtual void CreateListenOrDie(int backlog = gbacklog) = 0;
        virtual SockPtr Accepter(InetAddr *clientaddr) = 0;
        virtual bool Connector(const std::string &server_ip, uint16_t server_port) = 0;
        virtual int Sockfd() = 0;
        virtual void Close() = 0;

        virtual ssize_t Recv(std::string *out) = 0;
        virtual ssize_t Send(const std::string &in) = 0;

    public:
        void BuildListenSocket(uint16_t port)
        {
            CreateSocketOrDie();
            CreateBindOrDie(port);
            CreateListenOrDie();
        }
        bool BuildClientSocket(std::string &server_ip, uint16_t server_port)
        {
            CreateSocketOrDie();
            return Connector(server_ip, server_port);
        }
        // void BuildUdpSocket()
        // {}
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket()
        {
        }
        TcpSocket(int sockfd)
            : _sockfd(sockfd)
        {
        }
        ~TcpSocket()
        {
        }

        virtual void CreateSocketOrDie() override
        {
            // 创建套接字
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(FATAL, "create socket errror!\n");
                exit(SOCKET_ERROR);
            }
            int opt = 1;
            setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
            LOG(INFO, "create socket success, sockfd: %d\n", _sockfd); // 3
        }

        virtual void CreateBindOrDie(uint16_t port) override
        {
            // 由用户自己传入本机信息 port
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = INADDR_ANY; // 绑定任意ip,接收任意ip发来的数据.
            if (::bind(_sockfd, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                LOG(FATAL, "bind error!\n");
                exit(BIND_ERROR);
            }
            //LOG(INFO, "bind success!\n");
        }

        virtual void CreateListenOrDie(int backlog) override
        {
            // tcp是面向连接的,tcp要持续能获取连接,随时监听.
            if (::listen(_sockfd, backlog) < 0)
            {
                LOG(FATAL, "listen error!\n");
                exit(LISTEN_ERROR);
            }
            //LOG(INFO, "listen success!\n");
        }

        virtual SockPtr Accepter(InetAddr *clientaddr) override
        {
            // server获取新连接
            // 同时还要获得client的信息，传入输入型参数带出去。
            // 连接好后会获得一个新的套接字，我们要返回，且面向对象式返回。SockPtr就是指向Socket的指针，我们就把它当作一个套接字
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sockfd = ::accept(_sockfd, (struct sockaddr *)&client, &len);
            if (sockfd < 0)
            {
                LOG(WARNING, "accept error! errno: %d\n", errno);
                return nullptr;
            }
            *clientaddr = InetAddr(client);
            LOG(INFO, "get a new link! cilent info: %s, sockfd: %d\n", clientaddr->AddrStr().c_str(), _sockfd);

            return std::make_shared<TcpSocket>(sockfd); // c++14
        }

        virtual bool Connector(const std::string &server_ip, uint16_t server_port) override
        {
            // client连接
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(server_port);
            ::inet_pton(AF_INET, server_ip.c_str(), &server.sin_addr); //@@@?

            int n = ::connect(_sockfd, (struct sockaddr *)&server, sizeof(server));
            if (n < 0)
                return false;

            return true;
        }

        virtual int Sockfd()
        {
            return _sockfd;
        }

        virtual void Close()
        {
            if (_sockfd > 0)
            {
                ::close(_sockfd);
            }
        }

        virtual ssize_t Recv(std::string *out) override
        {
            char inbuffer[4096];
            ssize_t n = ::recv(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
            if (n > 0)
            {
                inbuffer[n] = 0;  // 当字符串
                *out += inbuffer; // 下一次读不会覆盖之前的内容。
            }
            return n;
        }

        virtual ssize_t Send(const std::string &in) override
        {
            return ::send(_sockfd, in.c_str(), in.size(), 0);
        }

    private:
        int _sockfd; // 可以是listen，可以是normal
    };

    // class UdpSocket : public Socket
    // {};
};