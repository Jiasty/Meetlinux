#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <pthread.h>
#include "InetAddr.hpp"
#include "Log.hpp"
#include "threadPool.hpp"
#include "lockGuardian.hpp"

using namespace log_ns;
using task_t = std::function<void()>;

// class User
// {

// };

class Route
{
public:
    Route()
    {
        pthread_mutex_init(&_mutex, nullptr);
    }

    void CheckOnlineUser(InetAddr &who)
    {
        LockGuard lock(&_mutex);
        for (auto &user : _onlineUser)
        {
            if (user == who) // 重载一下==
            {
                LOG(DEBUG, "%s is exists\n", who.AddrStr().c_str());
                return;
            }
        }
        LOG(DEBUG, "%s is not exists, add it\n", who.AddrStr().c_str());
        _onlineUser.push_back(who);
    }

    void Offline(InetAddr &who)
    {
        LockGuard lock(&_mutex);
        for (auto iter = _onlineUser.begin(); iter != _onlineUser.end(); iter++)
        {
            if (*iter == who)
            {
                LOG(DEBUG, "%s is offline\n", who.AddrStr().c_str());
                _onlineUser.erase(iter);
                break;
            }
        }
    }

    void ForwardHelper(int sockfd, const std::string message, InetAddr who) // 为何不加引用？
    {
        std::string send_message = "[" + who.AddrStr() + "]#" + message;
        LockGuard lock(&_mutex);
        for (auto &user : _onlineUser)
        {
            struct sockaddr_in peer = user.Addr();
            LOG(DEBUG, "send a message %s to %s", send_message.c_str(), user.AddrStr().c_str());
            ::sendto(sockfd, send_message.c_str(), send_message.size(), 0, (struct sockaddr *)&peer, sizeof(peer));
        }
    }

    void Forward(int sockfd, const std::string &message, InetAddr &who)
    {
        // 消息转发。

        // 1. 检查用户是否在 在线用户列表中。如果在，什么都不做，如果不在，自动添加到_onlineUser。
        CheckOnlineUser(who);

        // 1.1 message == "QUIT" "Q"
        if (message == "QUIT" || message == "Q")
        {
            Offline(who);
        }

        // 2. who此时已经在_onlineUser，开始向其他用户转发。利用线程池。
        // ForwardHelper(sockfd, message, who);
        task_t t = std::bind(&Route::ForwardHelper, this, sockfd, message, who);
        ThreadPool<task_t>::GetInstance()->Push(t);
    }

    ~Route()
    {
        pthread_mutex_destroy(&_mutex);
    }

private:
    std::vector<InetAddr> _onlineUser;
    pthread_mutex_t _mutex;
};
