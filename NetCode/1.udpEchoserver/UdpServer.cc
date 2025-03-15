#include <memory>

#include "UdpServer.hpp"

using namespace log_ns;

// ./udp_server local-port
// ./udp_server 8888
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " local_port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    EnableScreen();
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port); // 智能指针。回看
    usvr->InitServer();
    usvr->Start();

    return 0;
}