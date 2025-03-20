#include <memory>
#include "tcpServer.hpp"

// ./tcpserver 8888
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << "local_port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port);
    tsvr->Init();
    tsvr->Start();

    return 0;
}