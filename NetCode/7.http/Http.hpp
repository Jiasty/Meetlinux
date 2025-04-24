#pragma once
// #define TEST
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>

const static std::string base_sep = "\r\n";
const static std::string line_sep = ": ";
const static std::string blank_sep = " ";
const static std::string suffix_sep = ".";
const static std::string methodInurl_sep = "?";
const static std::string pref_path = "wwwroot"; // web根目录
const static std::string httpversion = "HTTP/1.0";
const static std::string homePage = "index2.html";
const static std::string _404Page = "404.html";

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
        reqstr.erase(0, retline.size() + base_sep.size());
        return retline.empty() ? base_sep : retline; // 有可能报头读完。
    }

    void ParseReqline()
    {
        // 利用sstream头文件@@@?
        std::stringstream ss(_reqLine);
        ss >> _method >> _url >> _version; // 类似于cin>>依次输入给它们三个，按空格分开。

        // /a/b/c.html  /login?user=xxx&passwd=12345 /register
        if (strcasecmp(_method.c_str(), "GET") == 0)
        {
            auto pos = _url.find(methodInurl_sep);
            if (pos != std::string::npos)
            {
                _bodyContent = _url.substr(pos + methodInurl_sep.size());
                _url.resize(pos);
            }
        }

        _path += _url;

        if (_path[_path.size() - 1] == '/')
        {
            _path += homePage; // 访问 / 时拼上首页。
        }

        auto pos = _path.rfind(suffix_sep);
        if (pos != std::string::npos)
        {
            _suffix = _path.substr(pos);
        }
        else
        {
            _suffix = ".default";
        }
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
        : _blankLine(base_sep),
          _path(pref_path)
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
            if (header.empty())
                break;
            else if (header == base_sep)
                break;
            _headers.emplace_back(header);
        } while (true);

        if (!reqstr.empty())
            _bodyContent = reqstr;

        // 我们需要各种属性，要保存一下。进一步反序列化。
        ParseReqline();
        ParseReqHeader();
    }

    std::string Url()
    {
        LOG(DEBUG, "client want url: %s \n", _url.c_str());
        return _url;
    }
    std::string Path()
    {
        LOG(DEBUG, "client want path: %s \n", _path.c_str());
        return _path;
    }
    std::string Suffix()
    {
        return _suffix;
    }
    std::string GetMethod()
    {
        LOG(DEBUG, "Client request method is %s\n", _method.c_str());
        return _method;
    }
    std::string GetReqcontent()
    {
        LOG(DEBUG, "Client request method is %s, args: %s, request path: %s\n",
            _method.c_str(), _bodyContent.c_str(), _path.c_str());
        return _bodyContent;
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
    std::string _path;
    std::string _suffix; // 资源后缀
    std::string _version;
    std::unordered_map<std::string, std::string> _headersKV;
};

class HttpResponse
{
public:
    HttpResponse()
        : _version(httpversion),
          _blankLine(blank_sep)
    {
    }

    void AddStatuscode(int code, const std::string &desc)
    {
        _status = code;
        _descOfstatus = desc;
    }
    void Addheader(const std::string &k, const std::string &v)
    {
        _headersKV[k] = v;
    }
    void AddBodyText(const std::string &body_text)
    {
        _resp_bodyContent = body_text;
    }

    std::string Serialize()
    {
        // 1.构建状态行
        _statusLine = _version + blank_sep + std::to_string(_status) + blank_sep + _descOfstatus + base_sep;
        // 2.构建响应报头
        for (auto &header : _headersKV)
        {
            std::string header_line = header.first + line_sep + header.second + base_sep;
            _resp_headers.push_back(header_line);
        }
        // 3.空行和内容行现成的。

        // 依次填入字符串。
        std::string respstr = _statusLine;
        for (auto &header : _resp_headers)
        {
            respstr += header;
        }
        respstr += base_sep;
        // respstr += _blankLine;
        respstr += _resp_bodyContent;
        return respstr;
    }

    ~HttpResponse()
    {
    }

private:
    std::string _statusLine;
    std::vector<std::string> _resp_headers; // 响应报头
    std::string _blankLine;
    std::string _resp_bodyContent;

    std::string _version;
    int _status;
    std::string _descOfstatus;
    std::unordered_map<std::string, std::string> _headersKV;
};

using func_t = std::function<HttpResponse(HttpRequest)>;

class HttpServer
{
private:
    std::string GetFileContent(const std::string &path)
    {
        //
        // 看看其他做法。
        std::ifstream in(path, std::ios::binary); // 图片视频等都应该以二进制打开。
        if (!in.is_open())
        {
            std::cout << path.c_str() << std::endl;
            return std::string();
        }
        in.seekg(0, in.end);
        int filesize = in.tellg(); // 告知我你的rw偏移量是多少
        in.seekg(0, in.beg);

        std::string content;
        content.resize(filesize);
        // in.read((char *)content.c_str(), filesize);
        in.read(&content[0], filesize);
        in.close();

        std::cout << "成功读取文件: " << path << " 大小: " << content.size() << " 字节" << std::endl;

        return content;
    }

public:
    HttpServer()
    {
        _mime_type.insert(std::make_pair(".html", "text/html"));
        _mime_type.insert(std::make_pair(".jpg", "image/jpeg"));
        _mime_type.insert(std::make_pair(".png", "image/png"));
        _mime_type.insert(std::make_pair(".default", "text/html"));

        _statusCode_to_desc.insert(std::make_pair(100, "Continue"));
        _statusCode_to_desc.insert(std::make_pair(200, "OK"));
        _statusCode_to_desc.insert(std::make_pair(201, "Created"));
        _statusCode_to_desc.insert(std::make_pair(301, "Moved Permanently"));
        _statusCode_to_desc.insert(std::make_pair(302, "Found"));
        _statusCode_to_desc.insert(std::make_pair(404, "Not Found"));
    }

    std::string HandlerRequest(std::string &reqstr)
    {
#ifdef TEST // 条件编译@@@？
        std::cout << "--------------------------" << std::endl;
        std::cout << reqstr;

        std::string resp = "HTTP/1.1 200 OK\r\n"; // 规定这么写。
        resp += "Content-Type: text/html\r\n";
        // 读取 HTML 文件内容
        std::ifstream file("./wwwroot/index.html");
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
        // 更具体的属性
        // std::string _method;
        // std::string _url;
        // std::string _path;
        // std::string _suffix; // 资源后缀
        // std::string _version;
        // std::unordered_map<std::string, std::string> _headersKV;
        std::cout << "---------------------------------------" << std::endl;
        std::cout << reqstr;
        std::cout << "---------------------------------------" << std::endl;

        HttpRequest req;
        HttpResponse resp;
        req.Deserialize(reqstr);
        req.GetReqcontent();
        // req.Print();
        // std::string url = req.Url();
        // std::string path = req.Path();
        if (req.Path() == "wwwroot/redir")
        {
            // 重定向处理
            std::string redir_path = "https://www.bilibili.com"; // 要写全以什么协议访问，不然会404.
            resp.AddStatuscode(302, _statusCode_to_desc[302]);
            resp.Addheader("Location", redir_path);
        }
        else if (!req.GetReqcontent().empty())
        {
            // 带参数，请求某个服务。
            if (isServiceExists(req.Path()))
            {
                resp = _service_list[req.Path()](req);
            }
        }
        else
        {
            std::string content = GetFileContent(req.Path());
            if (content.empty())
            {
                content = GetFileContent("wwwroot/404.html");
                resp.AddStatuscode(404, _statusCode_to_desc[404]);
                resp.Addheader("Content-Length", std::to_string(content.size())); //@@@?
                resp.Addheader("Content-Type", _mime_type[".html"]);
                resp.AddBodyText(content);
            }
            else
            {
                resp.AddStatuscode(200, _statusCode_to_desc[200]);
                resp.Addheader("Content-Length", std::to_string(content.size())); //@@@?
                resp.Addheader("Content-Type", _mime_type[req.Suffix()]);
                resp.Addheader("Set-Cookie", "username=Jiasty");
                resp.AddBodyText(content);
            }
        }

        return resp.Serialize();
#endif
    }

    void InsertService(const std::string serviceName, func_t f)
    {
        const std::string s = pref_path + serviceName;
        _service_list[s] = f;
    }
    bool isServiceExists(const std::string serviceName)
    {
        auto iter = _service_list.find(serviceName);
        if (iter == _service_list.end())
            return false;
        else
            return true;
    }
    ~HttpServer()
    {
    }

private:
    std::unordered_map<std::string, std::string> _mime_type; // Content-Type对照表
    std::unordered_map<int, std::string> _statusCode_to_desc;
    std::unordered_map<std::string, func_t> _service_list;
};