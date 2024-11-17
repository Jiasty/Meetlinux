#include <mysql/mysql.h>
#include <iostream>

int main() 
{
    std::cout << "libmysqlclient-dev installed successfully!" << mysql_get_client_info() << std::endl;
    return 0;
}