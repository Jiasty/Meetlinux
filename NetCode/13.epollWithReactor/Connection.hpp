#pragma once
#include <iostream>
#include <string>
#include <functional>


class Connection;
using handler_t = std::function<void(Connection *conn)>;

// 对fd进行管理,未来服务器皆是connection，listensock也是一样。
class Connection
{
public:
    Connection()
    {
    }

    void RegisterHandler(handler_t recever, handler_t sender, handler_t excepter)
    {
        // 外部设置回调方法。
        _handler_recver = recever;
        _handler_sender = sender;
        _handler_excepter = excepter;
    }

    ~Connection()
    {
    }

private:
    int sockfd;
    std::string _inbuffer;  // TODO
    std::string _outbuffer; // TODO

    handler_t _handler_recver;
    handler_t _handler_sender;
    handler_t _handler_excepter;
};