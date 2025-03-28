#include <iostream>
#include <string>


int main(int argc,  char *argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " + local_ip + server_port" << std::endl;
        exit(1);
    }
    std::string ip = argv[1];
    uint16_t port = std::stoi(argv[2]);


    return 0;
}
