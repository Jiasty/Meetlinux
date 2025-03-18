#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myThread.hpp"

using namespace ThreadMoudle;

int InitClient()
{
    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(1);
    }
    return sockfd;
}

void RecvMessage(int sockfd, const std::string &name)
{
    while (true) // 接收方不发消息时也一直在收消息。
    {
        struct sockaddr_in temp; // 占位符，此时不使用。
        socklen_t len = sizeof(temp);
        char buffer[1024];
        int m = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&temp, &len);
        if (m > 0)
        {
            buffer[m] = 0;
            std::cout << buffer << std::endl;
        }
        else
        {
            std::cerr << "recvfrom error\n" << std::endl;
            break;
        }
    }
}

void SendMessage(int sockfd, std::string serverip, uint16_t serverport, const std::string &name)
{
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    std::string client_profix = name + "# ";
    while (true)
    {
        std::string message;
        std::cout << client_profix;
        fflush(stdout);
        std::getline(std::cin, message);

        int n = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server)); // 要知道发给谁
        if (n < 0)
            break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        exit(1);
    }
    // 客户端未来一定要知道服务器的ip地址和端口号
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    int sockfd = InitClient();

    Thread recvr("recvr-thread", std::bind(RecvMessage, sockfd, std::placeholders::_1));
    Thread sender("sender-thread", std::bind(SendMessage, sockfd, server_ip, server_port, std::placeholders::_1));

    recvr.Start();
    sender.Start();

    recvr.Join();
    sender.Join();

    ::close(sockfd);

    return 0;
}