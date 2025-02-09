#include "mystdio.h"

const char* filename = "log.txt";

int main()
{
    myFILE* fp = my_fopen(filename, "w");
    if(fp == NULL) return 1;

    const char* message = "Hello world!\n";
    
    int cnt = 10;
    char buffer[128];
    while(cnt)
    {
        sprintf(buffer, "%s %d\n", message, cnt);
        my_fwrite(buffer, strlen(buffer), fp); //strlen() + 1不需要
        cnt--;
    }

    my_fclose(fp);
    return 0;
}