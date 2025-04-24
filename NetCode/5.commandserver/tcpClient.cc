#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ./tcpclient server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(1);
    }

    // 客户端不需要显示bind,但一定要有自己的IP和port,需要隐式bind,OS会自动bind sockfd,用自己的ip和随机端口号.
    // 客户端需要发起连接.什么时候自动bind?connec成功时.
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    ::inet_pton(AF_INET, server_ip.c_str(), &server.sin_addr); //?

    int n = ::connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (n < 0)
    {
        std::cerr << "connect socket error" << std::endl;
        exit(2);
    }

    while (true)
    {
        std::string message;
        std::cout << "Enter # ";
        std::getline(std::cin, message);

        ::write(sockfd, message.c_str(), message.size());

        char echobuffer[1024];
        n = ::read(sockfd, echobuffer, sizeof(echobuffer));
        if (n > 0)
        {
            echobuffer[n] = 0;
            std::cout << echobuffer << std::endl;
        }
        else
        {
            break;
        }
    }

    ::close(sockfd);
    return 0;
}