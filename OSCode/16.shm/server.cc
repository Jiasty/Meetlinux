#include "shm.hpp"
#include "namedPipe.hpp"

// server -> read 需要读数据就要主动建立通信，就要维护管道的生命周期。

int main()
{
    Shm shm(Gpath, Gproj_id, Creater);
    char *shmaddr = (char *)shm.Addr();
    // 2. 创建管道
    namedPipe fifo(commPath, Creater);
    fifo.OpenforRead();

    while(true)
    {
        std::string temp;
        fifo.readData(&temp);

        std::cout << "shm memory content: " << shmaddr << std::endl;
    }

    return 0;
}