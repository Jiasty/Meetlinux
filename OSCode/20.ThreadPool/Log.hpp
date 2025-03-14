#pragma once
#include <iostream>
#include <ctime>
#include <cstdarg>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include "lockGuardian.hpp"

#define MINITOR 1
#define FILE 2

const std::string file = "./log.txt";
pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;

namespace log_ns
{
    enum
    {
        DEBUG = 1, // 不初始化默认为0
        INFO,
        WARNING,
        ERROR,
        FATAL,
    };

    std::string LeveltoString(int level)
    {
        switch (level)
        {
        case 1:
            return "DEBUG";
        case 2:
            return "INFO";
        case 3:
            return "WARNING";
        case 4:
            return "ERROR";
        case 5:
            return "FATAL";
        default:
            return "UNKNONE";
        }
    }

    std::string GetcurTime()
    {
        time_t now = time(nullptr); // 获取时间戳，将其转化为我们认识的时间。
        struct tm *curTime = localtime(&now);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%d-%02d-%02d %02d:%02d:%02d",
                 curTime->tm_year + 1900,
                 curTime->tm_mon + 1,
                 curTime->tm_mday,
                 curTime->tm_hour,
                 curTime->tm_min,
                 curTime->tm_sec);
        return buffer;
    }

    class LogMessage
    {
    public:
        std::string _level;
        pid_t _id;
        std::string _filename;
        int _filenumber;
        std::string _curTime;
        std::string _message;
    };

    class Log
    {
    public:
        Log(const std::string &logfile = file)
            : _logfile(logfile),
              _type(MINITOR)
        {
        }
        void SwitchType(int type) // 切换打印模式
        {
            _type = type;
        }
        void FlushtoMinitor(LogMessage &log)
        {
            printf("[%s][%d][%s][%d][%s] %s\n",
                   log._level.c_str(),
                   log._id,
                   log._filename.c_str(),
                   log._filenumber,
                   log._curTime.c_str(),
                   log._message.c_str());
        }
        void FlushtoFile(LogMessage &log)
        {
            std::ofstream out(_logfile, std::ios::app); // 追加打开
            if (!out.is_open())
                return;
            char buffer[2048];
            snprintf(buffer, sizeof(buffer), "[%s][%d][%s][%d][%s] %s\n",
                     log._level.c_str(),
                     log._id,
                     log._filename.c_str(),
                     log._filenumber,
                     log._curTime.c_str(),
                     log._message.c_str());

            out.write(buffer, strlen(buffer)); // strlen不带\n
            out.close();
        }
        void FlushLog(LogMessage &log)
        {
            // 可以加过滤，过滤掉不想判断的错误等级

            LockGuard lock(&gmutex); // 线程安全
            switch (_type)
            {
            case MINITOR:
                FlushtoMinitor(log);
                break;
            case FILE:
                FlushtoFile(log);
                break;
            }
        }
        void writeMessage(std::string filename, int filenumber, int level, const char *format, ...)
        {
            LogMessage lg;
            lg._level = LeveltoString(level);
            lg._id = getpid();
            lg._filename = filename;
            lg._filenumber = filenumber;
            lg._curTime = GetcurTime(); // 年-月-日 时:分:秒

            //@@@用户传参可变参数可能会有问题，捕获一下。
            va_list ap; // char *的指针
            va_start(ap, format);
            char log_info[1024];
            vsnprintf(log_info, sizeof(log_info), format, ap); // 按照format格式将可变参数ap提出来放入log_info中
            va_end(ap);

            lg._message = log_info;

            // 打印log
            FlushLog(lg);
        }

        ~Log()
        {
        }

    private:
        int _type;            // 打印方式。
        std::string _logfile; // 向文件打印时的文件路径，多线程时它就是临界资.
    };

    Log lg;
    //@@@以后都不用手动传文件名和行号了。
#define LOG(Level, Format, ...)                                          \
    do                                                                   \
    {                                                                    \
        lg.writeMessage(__FILE__, __LINE__, Level, Format, ##__VA_ARGS__); \
    } while (0)
#define EnableScreen()          \
    do                          \
    {                           \
        lg.SwitchType(MINITOR); \
    } while (0)
#define EnableFILE()          \
    do                        \
    {                         \
        lg.SwitchType(FILE); \
    } while (0)
}
