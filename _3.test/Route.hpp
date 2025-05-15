#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "InetAddr.hpp"
#include "ThreadPool.hpp"
#include "LockGuard.hpp"

// class user
// {};

using task_t = std::function<void()>;

class Route
{
public:
    Route()
    {
        pthread_mutex_init(&_mutex, nullptr);
    }
    void CheckOnlineUser(InetAddr &who)
    {
        LockGuard lockguard(&_mutex);
        for (auto &user : _online_user)
        {
            if (user == who)
            {
                LOG(DEBUG, "%s is exists\n", who.AddrStr().c_str());
                return;
            }
        }
        LOG(DEBUG, "%s is not exists, add it\n", who.AddrStr().c_str());
        _online_user.push_back(who);
    }
    // for test

    void Offline(InetAddr &who)
    {
        LockGuard lockguard(&_mutex);
        auto iter = _online_user.begin();
        for (; iter != _online_user.end(); iter++)
        {
            if (*iter == who)
            {
                LOG(DEBUG, "%s is offline\n", who.AddrStr().c_str());
                _online_user.erase(iter);
                break;
            }
        }
    }

    void ForwardHelper(int sockfd, const std::string message, InetAddr who)
    {
        LockGuard lockguard(&_mutex);
        std::string send_message = "[" + who.AddrStr() + "]# " + message;
        for (auto &user : _online_user)
        {
            struct sockaddr_in peer = user.Addr();
            LOG(DEBUG, "Forward message to %s, message is %s\n", user.AddrStr().c_str(), send_message.c_str());
            ::sendto(sockfd, send_message.c_str(), send_message.size(), 0, (struct sockaddr *)&peer, sizeof(peer));
        }
    }

    void Forward(int sockfd, const std::string &message, InetAddr &who)
    {
        // 1. 该用户是否在 在线用户列表中呢？如果在，什么都不做，如果不在，自动添加到_online_user
        CheckOnlineUser(who);

        // 1.1 message == "QUIT" "Q"
        if (message == "QUIT" || message == "Q")
        {
            Offline(who);
        }

        // 2. who 一定在_online_user列表里面
        // ForwardHelper(sockfd, message);
        task_t t = std::bind(&Route::ForwardHelper, this, sockfd, message, who);
        ThreadPool<task_t>::GetInstance()->Equeue(t);
    }
    ~Route()
    {
        pthread_mutex_destroy(&_mutex);
    }

private:
    std::vector<InetAddr> _online_user;
    pthread_mutex_t _mutex;
};