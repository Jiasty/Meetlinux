#pragma once

class nocopy
{
    // 该类的作用？
    // 网络服务不建议拷贝对象。我们定义一个无法拷贝和赋值的父类，让其继承，实现单例。
public:
    nocopy() {}
    ~nocopy() {}
    nocopy(const nocopy &) = delete;
    const nocopy &operator=(const nocopy &) = delete;
};