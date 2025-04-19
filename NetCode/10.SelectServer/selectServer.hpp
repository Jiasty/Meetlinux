#pragma once
#include <iostream>
#include <sys/select.h>
#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace socket_ns;

class selectServer
{
public:
    selectServer(uint16_t port)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
    }
    void InitServer()
    {
    }

    void HandlerEvent(fd_set &rfds)
    {
        if (FD_ISSET(_listensock->Sockfd(), &rfds))
            ;
        {
            // 连接事件就绪，等同于读事件就绪。
            InetAddr addr;
            int sockfd = _listensock->Accepter(&addr); // 会不会阻塞?一定不会！
            if (sockfd > 0)
            {
                LOG(DEBUG, "get a new link, client info: %s\n", addr.AddrStr());
                // 已经获得了一个新的sockfd
                // 接下来能读取吗？不能，读取可能阻塞，不发请求，读取会阻塞。
                // 谁最清楚底层fd的数据是否就绪了呢？OS，用户通过select得知。
                // 想办法把新的fd添加给select，由select统一进行监管。怎么做到？？
            }
            else
            {
                return;
            }
        }
    }

    void Loop()
    {
        while (true)
        {
            // TODO
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(_listensock->Sockfd(), &rfds);

            struct timeval timeout = {30, 0};

            // _listensock->Accepter();
            // 不能直接accept，先等连接，再拷贝fd.我们把listensock和accept也看做IO类的函数，只关心新连接到来，等价于读事件就绪。
            int n = ::select(_listensock->Sockfd() + 1, &rfds, nullptr, nullptr, nullptr /*&timeout*/);
            switch (n)
            {
            case 0:
                LOG(DEBUG, "time out, %d.%d\n", timeout.tv_sec, timeout.tv_usec);
                break;
            case -1:
                LOG(ERROR, "select err\n");
            default:
                // LOG(DEBUG, "time out, %d.%d\n", timeout.tv_sec, timeout.tv_usec);
                LOG(INFO, "some events ready, n: %d\n", n); // 如果事件就绪但不处理，就会一直通知我有就绪的。
                HandlerEvent(rfds);
                sleep(3);
                break;
            }
        }
    }

    ~selectServer()
    {
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
};
