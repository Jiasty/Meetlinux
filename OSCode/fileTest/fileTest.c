#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ONE 1          // 1 0000 0001
#define TWO (1 << 1)   // 2 0000 0010
#define THREE (1 << 2) // 4 0000 0100
#define FOUR (1 << 3)  // 8 0000 1000

void printForOtherTerminal()
{
    int fd = open("/dev/pts/1", O_WRONLY | O_APPEND);
    if(fd < 0) return;

    const char* message = "hello hello hello!";
    write(fd, message, strlen(message));
}

void FILEandfd()
{
    //stdin、stdout、stderr都是FILE结构体的对象，里面都有描述fd的字段_fileno
    printf("stdin->fd: %d\n", stdin->_fileno);
    printf("stdout->fd: %d\n", stdout->_fileno);
    printf("stderr->fd: %d\n", stderr->_fileno);

    const char* message = "Jiasty!!!!!!!!!!!!!!!!!";

    //显示器文件已经打开，fd为1，使用write向显示器写入
    fprintf(stdout, "1111111111111"); //fprintf是写入语言级缓冲区的，不刷新会在结束时自动刷新，所以会看到111111在write的后面
    fflush(stdout); //手动刷新一下就会在前面了
    write(1, message, strlen(message));
}

void testDefine(int flag)
{
    if (flag & ONE)
        printf("one\n"); // 替换成其他功能
    if (flag & TWO)
        printf("two\n");
    if (flag & THREE)
        printf("three\n");
    if (flag & FOUR)
        printf("four\n");
}
void systemCallFunc()
{
    // int fd = open("log.txt", O_WRONLY | O_CREAT); //一般用于操作已经存在的文件
    int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666); // 打开并创建，带上权限
    // O_WRONLY | O_CREAT就相当于各种宏，|之后就相当于带上了不同功能

    const char *message = "Hello Linux!";
    write(fd, message, strlen(message));

    close(fd);

    if (fd < 0) return; // 成功则返回大于等于0的fd
}

void testCfileFunc()
{
    FILE *fp = fopen("test.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        return;
    }

    fprintf(fp, "write for file!");

    fclose(fp);
}

int main()
{
    printForOtherTerminal();
    //FILEandfd();
    //systemCallFunc();
    // testCfileFunc();

    return 0;
}