#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class InetAddr
{
private:
    void ToHost(const struct sockaddr_in &addr)
    {
        _ip = inet_ntoa(addr.sin_addr);
        _port = ntohs(addr.sin_port);
    }

public:
    InetAddr(const struct sockaddr_in &addr)
        : _addr(addr)
    {
        ToHost(addr);
    }
    std::string Ip()
    {
        return _ip;
    }
    uint16_t Port()
    {
        return _port;
    }
    ~InetAddr()
    {
    }

private:
    std::string _ip;
    uint16_t _port;
    struct sockaddr_in _addr;
};