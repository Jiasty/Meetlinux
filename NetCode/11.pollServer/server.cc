#include <iostream>
#include <memory>
#include "pollServer.hpp"

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
    std::unique_ptr<pollServer> svr = std::make_unique<pollServer>(port);
    svr->InitServer();
    svr->Loop();
    return 0;
}