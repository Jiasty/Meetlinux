#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>


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
    srand(time(nullptr) ^ getpid() ^ 18888); //种随机数种子，后续选择任务随机公平。
    tasks[0] = Task1;
    tasks[1] = Task2;
    tasks[2] = Task3;
}

int SelectTask()
{
    //随机返回一个任务下标，保证公平性。
    return rand() % taskNum;
}

void ExcuteTask(int number)
{
    if (number < 0 || number > taskNum - 1) return;
    tasks[number]();
}

void doWork()
{
    while (true)
    {
        int command = 0; // 存放父进程发送过来的任务码
        int n = read(0, &command, sizeof(command));
        if (n == 0)
        {
            std::cout << "child process:" << getpid() << " quit!" << std::endl;
            break;
        }
        else if (n == sizeof(int)) // read读到多少字节n就为多少
        {
            std::cout << "pid is : " << getpid() << " handler task" << std::endl;
            ExcuteTask(command);
        }
        sleep(1);
    }
}

// void doWork2()
// {
//     while (true)
//     {
//         int command = 0; // 存放父进程发送过来的任务码
//         int n = read(0, &command, sizeof(command));
//         if (n == 0)
//         {
//             std::cout << "child process:" << getpid() << " quit!" << std::endl;
//             break;
//         }
//         else if (n == sizeof(int)) // read读到多少字节n就为多少
//         {
//             std::cout << "pid is : " << getpid() << " handler task" << std::endl;
//             ExcuteTask(command);
//         }
//         sleep(1);
//     }
// }