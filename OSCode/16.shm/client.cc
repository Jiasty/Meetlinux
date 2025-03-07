#include "shm.hpp"
#include "namedPipe.hpp"

// client -> write

int main()
{
    Shm shm(Gpath, Gproj_id, User);
    shm.CleanShm();
    char *shmaddr = (char *)shm.Addr();
    sleep(3);

    // 2. 打开管道
    namedPipe fifo(commPath, User);
    fifo.OpenforWrite();

    // 当成string
    char ch = 'A';
    while (ch <= 'Z')
    {
        shmaddr[ch - 'A'] = ch;

        std::string temp = "wakeup";
        std::cout << "add " << ch << " into Shm, " << "wakeup reader" << std::endl;
        fifo.writeData(temp);
        sleep(2);
        ch++;
    }

    return 0;
}