#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <jsoncpp/json/json.h>

static const std::string sep = "\r\n";

// 设计一下协议的报头和报文的完整格式, 保证读取报文的完整性！！！
// "len"\r\n"{json}"\r\n --- 完整的报文， len 有效载荷的长度！
// \r\n: 区分len 和 json 串
// \r\n: 暂是没有其他用，打印方便.
// 添加报头
std::string Encode(const std::string &jsonstring)
{
    int len = jsonstring.size();
    std::string lenstr = std::to_string(len);
    return lenstr + sep + jsonstring + sep; //"len"\r\n"{json}"\r\n
}
// 解析工作
//  "le
//  "len"
//  "len"\r\n
//  "len"\r\n"{json}"\r\n
//  "len"\r\n"{j
//  "len"\r\n"{json}"\r\n"len"\r\n"{
//  "len"\r\n"{json}"\r\n
//  "len"\r\n"{json}"\r\n"len"\r\n"{json}"\r\n"len"\r\n"{json}"\r\n"len"\r\n"{json}"\r
std::string Decode(std::string &packagestream)
{
    auto pos = packagestream.find(sep);
    if (pos == std::string::npos) // 处于尾部，没找到换行符。
        return std::string();
    // 找到第一个换行，截取len, (]
    std::string lenstr = packagestream.substr(0, pos);
    int len = std::stoi(lenstr);

    int totalSize = lenstr.size() + len + sep.size() * 2; // 完整报文的长度 "len"\r\n"{json}"\r\n
    if (packagestream.size() < totalSize)
        return std::string();

    // 至少有一个完整的报文，开始提取。
    std::string jsonstr = packagestream.substr(pos + sep.size(), len);
    packagestream.erase(0, totalSize); // 提取了一个之后将它删除
    return jsonstr;
}

// struct request req = {10, 20, '+'}
// write(sockfd, &wreq, sizeof(req)); read(sockfd, &rreq, sizeof(req)); //可以这样做吗？部分场景可以。
class Request
{
public:
    Request()
    {
    }
    Request(int x, int y, char oper)
        : _x(x),
          _y(y),
          _operater(oper)
    {
    }

    bool Serialize(std::string *out)
    {
        // 将请求序列化
        // 自己实现序列化。
        // 使用现成的库。
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _operater;

        Json::FastWriter writer;
        std::string s = writer.write(root);
        *out = s;
        return true;
    }
    bool Deserialize(const std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool res = reader.parse(in, root);
        if (!res)
            return false;

        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _operater = root["oper"].asInt();

        return true;
    }

    void Print()
    {
        std::cout << _x << std::endl;
        std::cout << _y << std::endl;
        std::cout << _operater << std::endl;
    }

    int X()
    {
        return _x;
    }
    int Y()
    {
        return _y;
    }
    char Oper()
    {
        return _operater;
    }

    void SetValueAgain(int x, int y, char oper)
    {
        _x = x;
        _y = y;
        _operater = oper;
    }

    ~Request()
    {
    }

private:
    // 这就是定协议，规定好这么用。
    int _x;
    int _y;
    char _operater; //+ - * / %
};

class Response
{
public:
    Response()
        : _ret(0),
          _errno(0),
          _desc("success")
    {
    }

    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["ret"] = _ret;
        root["errno"] = _errno;
        root["desc"] = _desc;
        Json::FastWriter writer;
        std::string s = writer.write(root);
        *out = s;
        return true;
    }
    bool Deserialize(const std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool res = reader.parse(in, root);
        if (!res)
            return false;

        _ret = root["ret"].asInt();
        _errno = root["errno"].asInt();
        _desc = root["desc"].asString();
        return true;
    }

    void Setret(int ret)
    {
        _ret = ret;
    }
    void Seterrno(int err)
    {
        _errno = err;
    }
    void Setdesc(const std::string desc)
    {
        _desc = desc;
    }

    void PrintResult()
    {
        std::cout << "ret: " << _ret << ", code: " << _errno << ", desc: " << _desc << std::endl;
    }

    ~Response()
    {
    }

private:
    int _ret;
    int _errno; // 0:success 1:div zero 2:illegal
    std::string _desc;
};

class Factory
{
    // 工厂模式@@@ TODO
    // request和response对象创建很频繁，让工厂帮我们。
public:
    static std::shared_ptr<Request> BuildRequestDefault()
    {
        return std::make_shared<Request>();
    }
    static std::shared_ptr<Response> BuildResponseDefault()
    {
        return std::make_shared<Response>();
    }
};