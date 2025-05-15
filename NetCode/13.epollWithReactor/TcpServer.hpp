#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "Connection.hpp"

// 暂时叫Tcpserver
// 以往的listensock都是封装到tcp里的，tcp直接定型了对listensock做管理，我们现在需要让他统一对connection做管理。

class TcpServer
{
public:
    TcpServer()
    {
    }

    ~TcpServer()
    {
    }

private:
    std::unordered_map<int, Connection *> _connections; // fd -> connection  ev.data.fd得到fd
};