#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>

int gval = 100;
void test1()
{
    pid_t id = fork();

    if(id < 0) return;
    else if (id == 0)
    {
        //CHILD
        int cnt = 0;
        while(++cnt)
        {
            printf("I am CHILD process, pid:%d, ppid:%d, gval:%d, gval_add:%p\n", getpid(), getppid(), gval, &gval);
            if(cnt == 5) gval = 300;
            sleep(1);
        }
    }
    else
    {
        //PARENT
        while(1)
        {
            printf("I am PARENT process, pid:%d, ppid:%d, gval:%d, gval_add:%p\n", getpid(), getppid(), gval, &gval);
            sleep(1);
        }
    }
}

int main()
{
    
    test1();
    return 0;
}