#pragma once
#include <iostream>
#include <string>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace socket_ns;

class epollServer
{
    const static int epoll_create_num = 128;
    const static int events_num = 128; // 监听的最大连接数
public:
    epollServer(uint16_t port)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
        _epfd = ::epoll_create(epoll_create_num);
        if (_epfd < 0)
        {
            LOG(FATAL, "epoll_create error!\n");
            exit(1);
        }
        LOG(INFO, "epoll_create success, epfd: %d\n", _epfd);
    }

    void InitServer()
    {
        // 新链接到来，认为是读事件就绪。
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Sockfd(); // 监听套接字 为何要有??? 为了在事件就绪时得到是哪个fd就绪。
        // 必须先把listen套接字添加进epoll模型。
        // 监听套接字添加进epoll模型后，epoll会自动监控这个套接字的状态变化。
        int n = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock->Sockfd(), &ev);
        if (n < 0)
        {
            LOG(FATAL, "epoll_ctl error!\n");
            exit(1);
        }
        LOG(INFO, "epoll_ctl success, add a new sockfd: %d\n", _listensock->Sockfd());
    }

    std::string EventtoString(uint32_t event)
    {
        std::string eventStr = "";
        if (event & EPOLLIN)
            eventStr += "EPOLLIN ";
        if (event & EPOLLOUT)
            eventStr += "EPOLLOUT ";
        return eventStr;
    }

    void Acceptor()
    {
        // listensock读事件就绪
        InetAddr addr;
        int sockfd = _listensock->Accepter(&addr);
        if (sockfd < 0)
        {
            LOG(ERROR, "accept error!\n");
            return; // 继续处理下一个事件。
        }
        LOG(INFO, "get a new link! cilent info: %s, sockfd: %d\n", addr.AddrStr().c_str(), sockfd);

        // 得到了一个新的sockfd，能不能进行read、recv呢？不能，不清楚新连接上是否有数据，等底层有数据才读，才不会别阻塞。
        // epoll最清楚底层是否有数据。将新的sockfd添加到epoll模型中。调用接口，OS来管理。
        struct epoll_event ev;
        ev.data.fd = sockfd;
        ev.events = EPOLLIN;
        int n = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &ev);
        LOG(INFO, "epoll_ctl add new sockfd: %d, events: %s\n", sockfd, EventtoString(ev.events).c_str());
    }

    void HandlerIO(int fd)
    {
        // 其他sockfd读事件就绪。
        char buffer[4096]; // 读取直接清空，此处是最大的问题，不能保证读到的是一个完整的请求，或者为多个请求！！！引入reactor。
        // 每一个fd都应该有自己的buffer。并且引入协议才能区分报文完整性。
        int n = ::recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer;
            std::string response = "HTTP/1.0 200 OK\r\n";
            std::string content = "<html><body><h1>hello world</h1></body></html>";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
            response += "\r\n";
            response += content;

            ::send(fd, response.c_str(), response.size(), 0); // 可以直接发送吗?
        }
        else if (n == 0)
        {
            LOG(INFO, "client close, fd: %d\n", fd);
            // 1.从epoll模型中删除这个fd，不再关心该fd
            // 从epoll中移除fd，这个fd一定要是健康合法的，否则可能会移除出错!!!
            ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            // 2.连接关闭
            ::close(fd);
        }
        else
        {
            LOG(ERROR, "recv error!\n");
            ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            ::close(fd);
        }
    }

    void HandlerEvents(int n)
    {
        for (int i = 0; i < n; i++)
        {
            int fd = revs[i].data.fd;
            uint32_t event = revs[i].events;
            LOG(INFO, "%d 上有事件就绪, events: %s\n", fd, EventtoString(event).c_str());

            if (event & EPOLLIN)
            {
                // 将不同的事件派发给不同的模块。
                if (fd == _listensock->Sockfd())
                    Acceptor();
                else
                    HandlerIO(fd);
            }
        }
    }

    void Loop()
    {
        int timeout = -1; // 1s
        while (true)
        {
            // _listensock->Accepter(); 此时是阻塞式IO,我们要让epoll知道.
            // 事件通知，事件派发。
            int n = ::epoll_wait(_epfd, revs, events_num, timeout);
            switch (n)
            {
            case 0:
                LOG(INFO, "epoll_wait timeout...\n");
                break;
            case -1:
                // 将一个非法的描述符添加到epoll，不应该发生，应该移除。TODO
                LOG(FATAL, "epoll_wait error!\n");
                break;
            default:
                LOG(INFO, "epoll_wait success, epfd: %d\n", _epfd);
                // 处理事件
                HandlerEvents(n);
                break;
            }
        }
    }

    ~epollServer()
    {
        if (_epfd >= 0)
        {
            ::close(_epfd);
        }
        _listensock->Close();
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    int _epfd;
    struct epoll_event revs[events_num];
};