#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

const static std::string base_sep = "\r\n";
const static std::string line_sep = ": ";

class HttpRequest
{
private:
    std::string Getline(std::string &reqstr)
    {
        //\r\n
        //\r\nDATA
        auto pos = reqstr.find(base_sep);
        if (pos == std::string::npos)
            return std::string(); // 没找到。
        std::string retline = reqstr.substr(0, pos);
        reqstr.erase(0, pos + base_sep.size());
        return retline.empty() ? base_sep : retline; // 有可能报头读完。
    }

    void ParseReqline()
    {
        // 利用sstream头文件@@@?
        std::stringstream ss(_reqLine);
        ss >> _method >> _url >> _version; // 类似于cin>>依次输入给它们三个，按空格分开。
    }

    void ParseReqHeader()
    {
        for (auto &header : _headers)
        {
            auto pos = header.find(line_sep);
            if (pos == std::string::npos)
                continue;
            std::string k = header.substr(0, pos);
            std::string v = header.substr(pos + line_sep.size());
            if (k.empty() || v.empty())
                continue;
            _headersKV.insert(std::make_pair(k, v));
        }
    }

public:
    HttpRequest()
        : _blankLine(base_sep)
    {
    }
    void Deserialize(std::string &reqstr)
    {
        // 手写反序列化。
        _reqLine = Getline(reqstr);
        std::string header;
        do
        {
            header = Getline(reqstr);
            if (header.empty() || header == base_sep)
                break;
            _headers.emplace_back(header);
        } while (true);

        if (!reqstr.empty())
            _bodyContent = reqstr;

        // 我们需要各种属性，要保存一下。进一步反序列化。
        ParseReqline();
        ParseReqHeader();
    }

    void Print()
    {
        std::cout << "----------------------------" << std::endl;
        std::cout << "###" << _reqLine << std::endl;
        for (auto &header : _headers)
        {
            std::cout << "$$$" << header << std::endl;
        }
        std::cout << _blankLine;
        std::cout << "@@@" << _bodyContent << std::endl;

        std::cout << "Method: " << _method << std::endl;
        std::cout << "Url: " << _url << std::endl;
        std::cout << "Version: " << _version << std::endl;

        for (auto &header_kv : _headersKV)
        {
            std::cout << ">>>" << header_kv.first << "->" << header_kv.second << std::endl;
        }
    }

private:
    // 基本的httprequest的格式
    std::string _reqLine;
    std::vector<std::string> _headers; // 请求报头
    std::string _blankLine;
    std::string _bodyContent;

    // 更具体的属性
    std::string _method;
    std::string _url;
    std::string _version;
    std::unordered_map<std::string, std::string> _headersKV;
};
class HttpResponse
{
private:
    std::string _statusLine;
    std::vector<std::string> _resp_headers; // 请求报头
    std::string _blankLine;
    std::string _resp_bodyContent;
};

class HttpServer
{
public:
    HttpServer()
    {
    }

    std::string HandlerRequest(std::string &reqstr)
    {
#ifdef TEST // 条件编译@@@？
        std::cout << "--------------------------" << std::endl;
        std::cout << req;

        std::string resp = "HTTP/1.1 200 OK\r\n"; // 规定这么写。
        resp += "Content-Type: text/html\r\n";
        // 读取 HTML 文件内容
        std::ifstream file("index.html");
        std::string html_content;
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                html_content += line + "\n";
            }
            file.close();
        }
        else
        {
            std::cerr << "无法打开 HTML 文件" << std::endl;
            html_content = "<html><h1>文件未找到</h1></html>";
        }

        resp += "Content-Length: " + std::to_string(html_content.length()) + "\r\n";
        resp += "\r\n";
        resp += html_content;

        return resp;
#else
        HttpRequest req;
        req.Deserialize(reqstr);
        req.Print();
        return "";

#endif
    }

    ~HttpServer()
    {
    }

private:
};