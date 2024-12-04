#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>


#define CommLineSize 512
#define ZERO '\0'
#define SEP " "
#define NUM 32

char cwd[CommLineSize*2];
char *gArgv[NUM];
int lastcode = 0;

const char* GetUsername()
{
    const char* user = getenv("USER");
    if(user == NULL) return "None";
    return user;
}
const char* GetHostname()
{
    const char* hostname = getenv("HOSTNAME");
    //if(hostname == NULL) return "None";
    //return hostname; ubuntu下好像没有HOSTNAME
    return "VM-16-11-ubuntu";
}
const char* GetCwd()
{
    const char* cwd = getenv("PWD");
    if(cwd == NULL) return "None";
    return cwd;
}

void MakeandPrintCommLine()
{
    char line[CommLineSize]; //定义一个缓冲区
    const char* userName = GetUsername();
    const char* hostName = GetHostname();
    const char* cwd = GetCwd();

    //获取了所有信息后向缓冲区里写入
    snprintf(line, sizeof(line), "[%s@%s:%s]> ", userName, hostName, cwd);
    printf("%s", line);
    fflush(stdout); //line的内容都在stdout的缓冲区中，需要刷新一下
}

int GetUserCommand(char command[], size_t n)
{
    char *s = fgets(command, n, stdin); //复习fgets
    if(s == NULL) return -1;
    command[strlen(command)-1] = ZERO;
    return strlen(command); 
}


void SplitCommand(char command[], size_t n)
{
    (void)n;
    // "ls -a -l -n" -> "ls" "-a" "-l" "-n"
    gArgv[0] = strtok(command, SEP);
    int index = 1;
    while((gArgv[index++] = strtok(NULL, SEP))); // done, 故意写成=,表示先赋值，在判断. 分割之后，strtok会返回NULL，刚好让gArgv最后一个元素是NULL, 并且while判断结束
}

void ExecuteComm()
{
    pid_t id = fork();
    if(id < 0) exit(1);
    else if(id == 0)
    {
        // child
        execvp(gArgv[0], gArgv);
        exit(errno);
    }
    else
    {
        // fahter
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        if(rid > 0)
        {
            lastcode = WEXITSTATUS(status);
            if(lastcode != 0) printf("%s:%s:%d\n", gArgv[0], strerror(lastcode), lastcode);
        }
    }
}

int main()
{
    //1.制作打印命令行提示符int
    MakeandPrintCommLine();

    //2.获取用户指令
    char comm[CommLineSize]; //空格也要获取，所以不能直接用scanf获取
    int n = GetUserCommand(comm, sizeof(comm));
    if(n <= 0) return 1;

    //3.切割命令字符串
    SplitCommand(comm, sizeof(comm));

    //4.检查内建命令

    //5.执行命令
    ExecuteComm();

    return 0;
}