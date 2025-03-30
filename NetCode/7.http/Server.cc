#include "tcpServer.hpp"
#include "Http.hpp"

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
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&HttpServer::HandlerRequest, &hserver, std::placeholders::_1),
        port);
    tsvr->Start();

    return 0;
}