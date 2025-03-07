#include "namedPipe.hpp"

// client -> write

int main()
{
    namedPipe fifo(commPath, WRITER);
    if (fifo.OpenforWrite()) // 映射
    {
        while (true)
        {
            std::cout << "Please enter>";
            std::string message;
            std::getline(std::cin, message); //读取一行
            fifo.writeData(message);
        }
    }

    return 0;
}