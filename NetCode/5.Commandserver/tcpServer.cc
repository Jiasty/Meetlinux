#include <memory>
#include "tcpServer.hpp"
#include "Command.hpp"

// ./tcpserver 8888
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << "local_port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    Command cmdserver;
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&Command::HandlerCommand,
                  &cmdserver,
                  std::placeholders::_1,
                  std::placeholders::_2),
        port);
    tsvr->Init();
    tsvr->Start();

    return 0;
}