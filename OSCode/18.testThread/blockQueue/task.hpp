#pragma once

#include <iostream>
#include <functional>

// typedef std::function<void()> task_t;
using task_t = std::function<void()>;

class Task
{
public:
    Task()
    {
    }
    Task(int x, int y)
        : _x(x), _y(y)
    {
    }

    void Excute()
    {
        _ret = _x + _y;
    }

    std::string debug()
    {
        std::string msg = std::to_string(_x) + "+" + std::to_string(_y) + "=?";
        return msg;
    }

    std::string result()
    {
        std::string msg = std::to_string(_x) + "+" + std::to_string(_y) + "=" + std::to_string(_ret);
        return msg;
    }

private:
    int _x;
    int _y;
    int _ret;
};