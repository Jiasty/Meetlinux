#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ./udp_client server-ip server-port
// ./udp_client 127.0.0.1 8888
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    // 客户端未来一定要知道服务器的ip地址和端口号
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(1);
    }

    // client的端口号，一般不让用户自己自己设定，而是让client OS随机选择。
    // client需不需要bind自己的ip和端口号？一定要有，但是不用显示（传参）bind它的ip和端口。
    // client在首次向服务器发送数据时，OS会自动给client bind它自己的ip和端口!!!

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    while (1)
    {
        std::string message;
        std::cout << "Please enter> ";
        std::getline(std::cin, message);

        int n = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server)); // 要知道发给谁
        if (n > 0)
        {
            struct sockaddr_in temp; //占位符，此时不使用。
            socklen_t len = sizeof(temp);
            char buffer[1024];
            int m = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&temp, &len);
            if(m > 0)
            {
                buffer[m] = 0;
                std::cout << buffer << std::endl;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }

    }

    ::close(sockfd);

    return 0;
}