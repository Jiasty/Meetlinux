#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

void test1()
{
    printf("begin...\n");
    execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
    printf("end...\n");
}
int main()
{
    pid_t cid = fork();
    if(cid == 0)
    {
        char* const Argv[] = {"myproc", "-a", "-b", "-c", NULL};
        char* const Env[] = {"HHH = 111111111", "MMM = 2222222222", NULL};
        
        //putenv("HHH = 111111111"); //为何无法编译？
        extern char** environ;
        //execve("./myproc", Argv, Env);
        execve("./myproc", Argv, environ);
        //execl("./myproc", "myproc", "-a", NULL); //路径已经知道了myproc的位置，执行时就不用带./了

        //execlp("tree", "tree", NULL); //第一个tree表示文件名，第二个表示如何执行时，可执行程序的名字
        //char* const argv[] = {"ls", "-a", "-l", "--color", NULL};
        //execv("/usr/bin/ls", argv);
        //execvp("ls", argv);

        exit(123);//只有替换失败时退出码才会为123
    }

    //PARENT
    int status = 0;
    pid_t ret = waitpid(cid, &status, 0);
    if(ret > 0) printf("PARENT wait successfully!Exit_code:%d\n", WEXITSTATUS(status));
    
    //test1();

    return 0;
}