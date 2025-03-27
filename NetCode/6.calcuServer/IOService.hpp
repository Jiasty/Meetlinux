#pragma once
#include <iostream>
#include <functional>
#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Protocol.hpp"

using namespace socket_ns;
using namespace log_ns;

// 设计这样的回调函数using service_io_t = std::function<void(SockPtr, const InetAddr &)>;
// 以后都在这里更改任务，tcpServer已经规定好处理这样的任务，与tcpServer解耦。
class IOService
{
public:
    IOService()
    {
    }

    void IOExecute(SockPtr sock, InetAddr &addr)
    {
        // 长服务
        while (true)
        {
            std::string message;
            ssize_t n = sock->Recv(&message);
            if (n > 0)
            {
                LOG(INFO, "get message %s from %s\n", message.c_str(), addr.AddrStr().c_str());

                std::string hello = "hello";
                sock->Send(hello);
            }
            else if (n == 0)
            {
                // 读到文件结尾, 客户端退出.
                LOG(INFO, "client %s quit!\n", addr.AddrStr().c_str());
                break;
            }
            else
            {
                LOG(ERROR, "read error: %s\n", addr.AddrStr().c_str());
                break;
            }
        }
    }

    ~IOService()
    {
    }
};