#pragma once
#include <iostream>
#include <string>
#include <poll.h>
#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace socket_ns;

class pollServer
{
    const static int gnum = sizeof(fd_set) * 8;
    const static int gdefaultfd = -1;

public:
    pollServer(uint16_t port)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
    }
    void InitServer()
    {
        for (int i = 0; i < gnum; i++)
        {
            fd_events[i].fd = gdefaultfd;
            fd_events[i].events = 0;
            fd_events[i].revents = 0;
        }
        fd_events[0].fd = _listensock->Sockfd();
        fd_events[0].events = POLLIN;
    }

    void Acceptor()
    {
        // 连接事件就绪，等同于读事件就绪。
        InetAddr addr;
        int sockfd = _listensock->Accepter(&addr); // 会不会阻塞?一定不会！
        if (sockfd > 0)
        {
            LOG(DEBUG, "get a new link, client info: %s\n", addr.AddrStr().c_str());
            // 已经获得了一个新的sockfd
            // 接下来能读取吗？不能，读取可能阻塞，不发请求，读取会阻塞。
            // 谁最清楚底层fd的数据是否就绪了呢？OS，用户通过select得知。
            // 想办法把新的fd添加给select，由select统一进行监管。怎么做到？？新的fd添加到fd_array。
            bool flag = false;
            for (int pos = 1; pos < gnum; pos++)
            {
                if (fd_events[pos].fd == gdefaultfd)
                {
                    flag = true;
                    fd_events[pos].fd = sockfd;
                    fd_events[pos].events = POLLIN;
                    LOG(INFO, "add %d to fd_array success!\n", sockfd);
                    break;
                }
            }
            if (!flag)
            {
                LOG(WARNING, "Server is full!\n");
                ::close(sockfd);
                // 可以扩容再添加。
            }
        }
    }

    void HandlerIO(int i)
    {
        // 下面的读写不对，不能保证读取的一个完整的报文。
        // 普通fd，正常的读写。
        char buffer[1024];
        ssize_t n = ::recv(fd_events[i].fd, buffer, sizeof(buffer) - 1, 0); // 不会阻塞。
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client say# " << buffer << std::endl;
            std::string content = "<html><body><h1>hello bite</h1></body></html>";
            std::string echo_str = "HTTP/1.0 200 OK\r\n";
            echo_str += "Content-Type: text/html\r\n";
            echo_str += "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n";
            echo_str += content;
            // echo_str += buffer;
            ::send(fd_events[i].fd, echo_str.c_str(), echo_str.size(), 0); // TODO 可能不具备写条件。
        }
        else if (n == 0)
        {
            LOG(INFO, "client quit!\n");
            ::close(fd_events[i].fd);
            fd_events[i].fd = gdefaultfd; // 不再关心该fd。
            fd_events[i].events = 0;
            fd_events[i].revents = 0;
        }
        else
        {
            LOG(ERROR, "recv error!\n");
            ::close(fd_events[i].fd);
            fd_events[i].fd = gdefaultfd; // 不再关心该fd。
            fd_events[i].events = 0;
            fd_events[i].revents = 0;
        }
    }

    // 一定会存在大量fd就绪，可能是普通sockfd，可能是listensockfd。
    void HandlerEvent()
    {
        // 事件派发。
        for (int i = 0; i < gnum; i++)
        {
            if (fd_events[i].fd == gdefaultfd)
                continue;
            // fd一定是合法的fd
            // 合法的fd不一定就绪
            if (fd_events[i].revents & POLLIN)
            {
                // 读事件就绪。是listensock还是普通sockfd？
                if (fd_events[i].fd == _listensock->Sockfd())
                {
                    Acceptor();
                }
                else
                {
                    HandlerIO(i);
                }
            }
        }
    }

    void Loop()
    {
        int timeout = 1000;
        while (true)
        {
            // _listensock->Accepter();
            // 不能直接accept，先等连接，再拷贝fd.我们把listensock和accept也看做IO类的函数，只关心新连接到来，等价于读事件就绪。
            int n = ::poll(fd_events, gnum, timeout);
            switch (n)
            {
            case 0:
                LOG(DEBUG, "time out!\n");
                break;
            case -1:
                LOG(ERROR, "poll err\n");
            default:
                LOG(INFO, "some events ready, n: %d\n", n); // 如果事件就绪但不处理，就会一直通知我有就绪的。
                HandlerEvent();
                PrintDebug();
                // sleep(3);
                break;
            }
        }
    }

    void PrintDebug()
    {
        std::cout << "fd list: ";
        for (auto fds : fd_events)
        {
            if (fds.fd == gdefaultfd)
                continue;
            std::cout << fds.fd << ":" << fds.events << " ";
        }
        std::cout << std::endl;
    }

    ~pollServer() {}

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    struct pollfd fd_events[gnum];
};