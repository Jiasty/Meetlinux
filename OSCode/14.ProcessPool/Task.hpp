#pragma once

#include <iostream>

#define taskNum 3

typedef void (*task_t)(); //定义了一个返回值为空，参数为空的函数指针类型
//void (*task_t)(); 此时task_t只是一个函数指针。

void Task1()
{
    std::cout << "Task 1" << std::endl;
}

void Task2()
{
    std::cout << "Task 2" << std::endl;
}

void Task3()
{
    std::cout << "Task 3" << std::endl;
}

task_t tasks[taskNum];

void LoadTasks()
{
    tasks[0] = Task1;
    tasks[1] = Task2;
    tasks[2] = Task3;
}

void ExcuteTask(int number)
{
    if (number < 0 || number > 2) return;
    tasks[number]();
}
