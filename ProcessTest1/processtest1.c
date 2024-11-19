#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
    pid_t id = getpid();

    while(1)
    {
        printf("This is a process, pid:%d\n", id);
        sleep(1);
    }

    return 0;
}
