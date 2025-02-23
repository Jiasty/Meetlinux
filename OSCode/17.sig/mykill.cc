#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: ./mykill + signum + pid" << std::endl;
        return 1;
    }
    int signum = std::stoi(argv[1]);
    pid_t pid = std::stoi(argv[2]);
    int n = kill(pid, signum);
    if(n != 0) std::cerr << "kill err" << std::endl;
    return 0;
}