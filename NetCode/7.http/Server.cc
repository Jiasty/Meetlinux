#include "tcpServer.hpp"
#include "Http.hpp"

HttpResponse Login(HttpRequest req)
{
    HttpResponse resp;
    std::cout << "外部已经拿到了参数了: " << std::endl;
    req.GetReqcontent();
    std::cout << "####################### " << std::endl;
    resp.AddStatuscode(200, "OK");
    resp.AddBodyText("<html><h1>result done!</h1></html>");

    // username=helloworld&userpasswd=123456

    // 创建子进程将任务交给其他语言处理！
    // 1. pipe
    // 2. dup2
    // 3. fork();
    // 4. exec* -> python, PHP, 甚至是Java！

    return resp;
}

// ./server 8888
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " + loacl_port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    HttpServer hserver;
    hserver.InsertService("/login", Login); // 注册登录服务

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&HttpServer::HandlerRequest, &hserver, std::placeholders::_1),
        port);
    tsvr->Start();

    return 0;
}