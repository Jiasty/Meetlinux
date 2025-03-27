#pragma once
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

// 设计一下协议的报头和报文的完整格式
// "len"\r\n"{json}"\r\n --- 完整的报文， len 有效载荷的长度！
// \r\n: 区分len 和 json 串
// \r\n: 暂是没有其他用，打印方便.
// 添加报头


// struct request req = {10, 20, '+'}
// write(sockfd, &wreq, sizeof(req)); read(sockfd, &rreq, sizeof(req)); //可以这样做吗？部分场景可以。
class Request
{
public:
    Request()
    {
    }

    void Serialize(std::string *out)
    {
        // 自己实现序列化。
        // 使用现成的库。
    }
    void Deserialize(const std::string &in)
    {
    }

    ~Request()
    {
    }

private:
    // 这就是定协议，规定好这么用。
    int x;
    int y;
    char operater; //+ - * / %
};

class Response
{
public:
    Response()
    {
    }

    void Serialize(std::string *out)
    {
    }
    void Deserialize(const std::string &in)
    {
    }

    ~Response()
    {
    }

private:
    int ret;
    int errno; // 0:success 1:div zero 2:illegal
};