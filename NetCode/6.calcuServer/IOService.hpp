#pragma once
#include <iostream>
#include <functional>
#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Protocol.hpp"

using namespace socket_ns;
using namespace log_ns;

using process_t = std::function<std::shared_ptr<Response>(std::shared_ptr<Request>)>; //@@@
// 它的任务就是将结构化的请求变成结构化的响应。

// 设计这样的回调函数using service_io_t = std::function<void(SockPtr, const InetAddr &)>;
// 以后都在这里更改任务，tcpServer已经规定好处理这样的任务，与tcpServer解耦。
class IOService
{
public:
    IOService(process_t process)
        : _process(process)
    {
    }

    void IOExecute(SockPtr sock, InetAddr &addr)
    {
        // 长服务
        std::string packagestreamQueue; // 一定放while外
        while (true)
        {
            // 1.读取
            ssize_t n = sock->Recv(&packagestreamQueue);
            if (n <= 0)
            {
                LOG(ERROR, "read error or client %s quit!\n", addr.AddrStr().c_str());
                break;
            }
            // 读成功，不能保证是一个完整的报文。
            // LOG(INFO, "get packagestream %s from %s\n", packagestream.c_str(), addr.AddrStr().c_str());

            // 2.报文解析
            std::string packstr = Decode(packagestreamQueue);
            if (packstr.empty())
                continue; // 继续接收

            // 此时就是一个完整的报文
            auto request = Factory::BuildRequestDefault();
            // 3.反序列化
            request->Deserialize(packstr);
            // 4.业务处理(NetCalcu.hpp)
            auto response = _process(request); // 通过请求，得到应答。
            // 5.序列化应答
            std::string respjson;
            response->Serialize(&respjson);
            // 6.添加报头
            respjson = Encode(respjson);
            // 7.发送应答
            sock->Send(respjson);
        }
    }

    ~IOService()
    {
    }

private:
    process_t _process;
};