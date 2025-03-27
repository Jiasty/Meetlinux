#include "tcpServer.hpp"
#include "IOService.hpp"

// ./server 8888
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " + loacl_port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    // 需要这样的回调函数using service_io_t = std::function<void(SockPtr, const InetAddr &)>;
    IOService service;
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&IOService::IOExecute, &service, std::placeholders::_1, std::placeholders::_2),
        port);
    tsvr->Start();

    return 0;
}