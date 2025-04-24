#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <set>
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace log_ns;

class Command
{
public:
    Command()
    {
        //白名单
        _safecommand.insert("ls");
        _safecommand.insert("pwd");
        _safecommand.insert("whoami");
        _safecommand.insert("touch");
        _safecommand.insert("tree");
    }
    ~Command()
    {
    }
    // BUG: ls;rm -rf / 其实也能执行的
    bool safeCheck(const std::string &cmdstr)
    {
        for (auto &cmd : _safecommand)
        {
            if (strncmp(cmd.c_str(), cmdstr.c_str(), cmd.size()) == 0)
                return true;
        }
        return false;
    }

    std::string Execute(const std::string &cmdstr)
    {
        // 处理命令
        if (!safeCheck(cmdstr))
        {
            return "dangerous";
        }
        std::string ret;
        FILE *fp = popen(cmdstr.c_str(), "r");
        if (fp)
        {
            char line[1024];
            while (fgets(line, sizeof(line), fp)) //@@
            {
                ret += line;
            }
            return ret.empty() ? "success" : ret; // 特处理touch这种无返回值的
        }
        return "execute error";
    }

    void HandlerCommand(int sockfd, InetAddr addr)
    {
        while (true)
        {
            // 长服务
            char commbuf[1024];                                          // 当字符串
            ssize_t n = ::recv(sockfd, commbuf, sizeof(commbuf) - 1, 0); // TODO 有小问题
            if (n > 0)
            {
                commbuf[n] = 0;
                LOG(INFO, "get a Command %s from %s\n", commbuf, addr.AddrStr().c_str());
                std::string ret = Execute(commbuf);
                ::send(sockfd, ret.c_str(), ret.size(), 0);
            }
            else if (n == 0)
            {
                // 读到文件结尾,客户端退出.
                LOG(INFO, "client %s quit!\n", addr.AddrStr().c_str());
                break;
            }
            else
            {
                LOG(ERROR, "read error: %s\n", addr.AddrStr().c_str());
                break;
            }
        }
        ::close(sockfd);
    }

private:
    std::set<std::string> _safecommand;
};