#include "namedPipe.hpp"

// server -> read 需要读数据就要主动建立通信，就要维护管道的生命周期。

int main()
{
    namedPipe fifo(commPath, READER);
    if (fifo.OpenforRead()) // 映射
    {
        while (true)
        {
            std::string message;
            int n = fifo.readData(&message);
            if (n > 0)
            {
                std::cout << "Client Say> " << message << std::endl;
            }
            else if (n == 0)
            {
                std::cout << "Client quit, Server Too!" << std::endl;
                break;
            }
            else
            {
                std::cout << "fifo.ReadNamedPipe Error" << std::endl;
                break;
            }
        }
    }

    return 0;
}