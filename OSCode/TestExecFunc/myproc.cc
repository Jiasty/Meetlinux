#include<iostream>

int main(int argc, char* argv[], char* env[])
{
    for(int i = 0; i < argc; i++)
    {
        printf("argv[%d]->%s\n", i, argv[i]);
    }
    printf("--------------------------------\n");
    for(int i = 0; env[i]; i++)
    {
        printf("env[%d]->%s\n", i, env[i]);
    }

    std::cout << "I am C++\n" << std::endl;
    std::cout << "I am C++\n" << std::endl;
    std::cout << "I am C++\n" << std::endl;
    return 0;
}