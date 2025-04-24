#pragma once
#include <iostream>
#include <string>
#include <sys/select.h>
#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace socket_ns;

class selectServer
{
    const static int gnum = sizeof(fd_set) * 8;
    const static int gdefaultfd = -1;

public:
    selectServer(uint16_t port)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
    }
    void InitServer()
    {
        for (int i = 0; i < gnum; i++)
        {
            fd_array[i] = gdefaultfd;
        }
        fd_array[0] = _listensock->Sockfd();
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
                if (fd_array[pos] == gdefaultfd)
                {
                    flag = true;
                    fd_array[pos] = sockfd;
                    LOG(INFO, "add %d to fd_array success!\n", sockfd);
                    break;
                }
            }
            if (!flag)
            {
                LOG(WARNING, "Server is full!\n");
                ::close(sockfd);
            }
        }
    }

    void HandlerIO(int i)
    {
        // 下面的读写不对，不能保证读取的一个完整的报文。
        // 普通fd，正常的读写。
        char buffer[1024];
        ssize_t n = ::recv(fd_array[i], buffer, sizeof(buffer) - 1, 0); // 不会阻塞。
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
            ::send(fd_array[i], echo_str.c_str(), echo_str.size(), 0); // TODO 可能不具备写条件。
        }
        else if (n == 0)
        {
            LOG(INFO, "client quit!\n");
            ::close(fd_array[i]);
            fd_array[i] = gdefaultfd; // 不再关心该fd。
        }
        else
        {
            LOG(ERROR, "recv error!\n");
            ::close(fd_array[i]);
            fd_array[i] = gdefaultfd; // 不再关心该fd。
        }
    }

    // 一定会存在大量fd就绪，可能是普通sockfd，可能是listensockfd。
    void HandlerEvent(fd_set &rfds)
    {
        // 事件派发。
        for (int i = 0; i < gnum; i++)
        {
            if (fd_array[i] == gdefaultfd)
                continue;
            // fd一定是合法的fd
            // 合法的fd不一定就绪
            if (FD_ISSET(fd_array[i], &rfds))
            {
                // 读事件就绪。是listensock还是普通sockfd？
                if (fd_array[i] == _listensock->Sockfd())
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
        while (true)
        {
            // 1.文件描述符进行初始化。
            fd_set rfds;
            FD_ZERO(&rfds);

            // 2.合法的fd添加到rfds中,并且维护最大的fd。
            int max_fd = gdefaultfd; // 供当select参数
            for (int i = 0; i < gnum; i++)
            {
                if (fd_array[i] == gdefaultfd)
                    continue;
                FD_SET(fd_array[i], &rfds);
                if (max_fd < fd_array[i])
                    max_fd = fd_array[i];
            }

            struct timeval timeout = {30, 0};

            // _listensock->Accepter();
            // 不能直接accept，先等连接，再拷贝fd.我们把listensock和accept也看做IO类的函数，只关心新连接到来，等价于读事件就绪。
            int n = ::select(max_fd + 1, &rfds, nullptr, nullptr, nullptr /*&timeout*/);
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
                PrintDebug();
                // sleep(3);
                break;
            }
        }
    }

    void PrintDebug()
    {
        std::cout << "fd list: ";
        for (auto fd : fd_array)
        {
            if (fd == gdefaultfd)
                continue;
            std::cout << fd << " ";
        }
        std::cout << std::endl;
    }

    ~selectServer() {}
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    // select正常工作，需要借助一个辅助数组，来保存所有合法的fd。
    int fd_array[gnum];
};