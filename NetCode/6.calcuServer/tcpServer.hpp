#pragma once
#include <functional>
#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"


using namespace socket_ns;
static const int gport = 8888;
using service_io_t = std::function<void(SockPtr, InetAddr &)>;

class TcpServer
{
private:
    static void *Execute(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *tp = static_cast<ThreadData *>(args);
        tp->_self->_service(tp->_sockfd, tp->_addr); // 回调函数
        tp->_sockfd->Close();
        delete tp;
        return nullptr;
    }

    class ThreadData
    {
    public:
        SockPtr _sockfd;
        TcpServer *_self;
        InetAddr _addr;

    public:
        ThreadData(SockPtr sockfd, TcpServer *self, const InetAddr &addr)
            : _sockfd(sockfd),
              _self(self),
              _addr(addr)
        {
        }
    };

public:
    TcpServer(service_io_t service, const uint16_t port = gport)
        : _listensock(std::make_shared<TcpSocket>()),
          _port(port),
          _isrunning(false),
          _service(service)
    {
        _listensock->BuildListenSocket(_port); // 就是Init();
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            InetAddr client;
            SockPtr newsockfd = _listensock->Accepter(&client);
            if (newsockfd == nullptr)
                continue;
            LOG(INFO, "get a new link! cilent info: %s, sockfd: %d\n", client.AddrStr().c_str(), newsockfd->Sockfd());

            // version-2 多线程版本(推荐)
            pthread_t thread;
            ThreadData *td = new ThreadData(newsockfd, this, client);
            pthread_create(&thread, nullptr, Execute, (void *)td);
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
    SockPtr _listensock; // 替换为套接字对象
    uint16_t _port;
    bool _isrunning;
    service_io_t _service; // 回调函数，交给上层传入。
};