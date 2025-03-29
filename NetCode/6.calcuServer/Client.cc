#include <iostream>
#include <string>
#include <ctime>
#include <unistd.h>
#include "Socket.hpp"
#include "Protocol.hpp"

using namespace socket_ns;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " + local_ip + server_port" << std::endl;
        exit(1);
    }
    std::string ip = argv[1];
    uint16_t port = std::stoi(argv[2]);

    SockPtr sock = std::make_shared<TcpSocket>();
    if (!sock->BuildClientSocket(ip, port))
    {
        std::cerr << "connect error!" << std::endl;
        exit(1);
    }

    srand(time(nullptr) ^ getpid());
    const std::string opers = "+-*/%!";

    std::string packagestreamQueue;
    while (true)
    {
        // 构建数据
        int x = rand() % 20;
        usleep(1000);
        int y = rand() % 20;
        usleep(1000);
        char oper = opers[y % opers.size()];

        // 构建请求
        auto req = Factory::BuildRequestDefault();
        req->SetValue(x, y, oper);

        // 序列化
        std::string reqstr;
        req->Serialize(&reqstr);

        // 添加报头
        std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
        reqstr = Encode(reqstr);
        std::cout << "request string:\n"
                  << reqstr << std::endl;

        // 发送数据
        sock->Send(reqstr);

        while (true) // 保证读取到完整应答
        {
            // 读取应答
            ssize_t n = sock->Recv(&packagestreamQueue);
            if (n <= 0)
            {
                // LOG(ERROR, "read error or client %s quit!\n", addr.AddrStr().c_str());
                break;
            }
            std::string packstr = Decode(packagestreamQueue);
            if (packstr.empty())
                continue; // 继续接收

            std::cout << "packstr: \n"
                      << packstr << std::endl;

            // 此时就是一个完整的报文
            auto response = Factory::BuildResponseDefault();
            // 反序列化
            response->Deserialize(packstr);

            // 打印结果
            response->PrintResult();
            break;
        }
        sleep(1);
    }
    sock->Close();

    return 0;
}
