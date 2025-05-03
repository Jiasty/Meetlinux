#include <memory>
#include "epollServer.hpp"

// ./server 8888
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " + loacl_port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);
    EnableScreen();
    std::unique_ptr<epollServer> svr = std::make_unique<epollServer>(port);
    svr->InitServer();
    svr->Loop();
    return 0;
}