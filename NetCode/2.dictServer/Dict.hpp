#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include "Log.hpp"

using namespace log_ns;

const static std::string sep = ": ";

class Dict
{
private:
    void LoadDict(const std::string path)
    {
        // 将path路径下的字典加载进unordered_map
        //@c++文件操作，回看
        std::ifstream in(path);
        if (!in.is_open())
        {
            LOG(FATAL, "open %s error!\n", path.c_str());
            exit(1);
        }

        std::string line;
        while (std::getline(in, line))
        {
            LOG(DEBUG, "load info: %s , success\n", line.c_str());
            if (line.empty()) // 空word
                continue;
            auto pos = line.find(sep);    //@
            if (pos == std::string::npos) // 无k-v
                continue;
            std::string key = line.substr(0, pos); // substr@
            if (key.empty())
                continue;
            std::string value = line.substr(pos + sep.size());
            if (value.empty())
                continue;

            _dict.insert(std::make_pair(key, value));
        }
        LOG(INFO, "load %s done\n", path.c_str());
        in.close();
    }

public:
    Dict(const std::string path)
        : _path(path)
    {
        LoadDict(_path);
    }

    std::string TranslateWords(std::string word)
    {
        if (word.empty())
            return "None";
        auto iterator = _dict.find(word); // 返回迭代器
        if (iterator == _dict.end())
            return "None";
        else
            return iterator->second;
    }

    ~Dict()
    {
    }

private:
    std::unordered_map<std::string, std::string> _dict;
    std::string _path;
};
