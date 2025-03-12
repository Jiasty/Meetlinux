#pragma once

#include<iostream>
#include<functional>

// typedef std::function<void()> task_t;
// using task_t = std::function<void()>;

// void Download()
// {
//     std::cout << "我是一个下载的任务" << std::endl;
// }


// 要做加法
class Task
{
public:
    Task()
    {
    }
    Task(int x, int y) : _x(x), _y(y)
    {
    }
    void Excute()
    {
        _result = _x + _y;
    }
    void operator ()()
    {
        Excute();
    }
    std::string debug()
    {
        std::string msg = std::to_string(_x) + "+" + std::to_string(_y) + "=?";
        return msg;
    }
    std::string result()
    {
        std::string msg = std::to_string(_x) + "+" + std::to_string(_y) + "=" + std::to_string(_result);
        return msg;
    }

private:
    int _x;
    int _y;
    int _result;
};