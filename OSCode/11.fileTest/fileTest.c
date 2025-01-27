#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ONE 1          // 1 0000 0001
#define TWO (1 << 1)   // 2 0000 0010
#define THREE (1 << 2) // 4 0000 0100
#define FOUR (1 << 3)  // 8 0000 1000

const char *filename = "log.txt";

void Redirect()
{
    // // C
    // printf("hello printf\n");
    // fprintf(stdout, "hello fprintf\n");
    // // system call
    // const char *msg = "hello write\n";
    // write(1, msg, strlen(msg));
    // fork(); //???

    // int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    // int fd = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0666);
    int fd = open("/dev/pts/2", O_CREAT | O_WRONLY | O_TRUNC, 0666); //向另外一个中断打印
    if(fd < 0)
    {
       perror("open");
       return;
    }
    dup2(fd, 1); //使用系统调用实现重定向
    printf("hello world\n");
    fprintf(stdout, "hello world\n");
    fflush(stdout);

    // //close(0);
    // //close(2);
    // //close(1); //手动关闭1，实现重定向
    // int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    // if(fd < 0)
    // {
    //    perror("open");
    //    return;
    // }

    // printf("printf, fd: %d\n", fd);
    // fprintf(stdout, "fprintf, fd: %d\n", fd);
    // fflush(stdout); //当关闭1后fd分配为了1，此时打印数据就是往log.txt里打印，但是最先是在语言级缓冲区，需要刷新一下。

    close(fd);
}

void GetFileInfo()
{
    int fd1 = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666); // 打开并创建，带上权限
    const char *message = "hello hello hello!";
    write(fd1, message, strlen(message));
    close(fd1);

    struct stat st; //输出型参数，stat()函数调用后将文件属性信息存入struct stat对象中
    if (stat(filename, &st) != 0) return;
    printf("file size: %lu\n", st.st_size);

    int fd2 = open(filename, O_RDONLY);
    if (fd2 < 0) return;

    char *file_buffer = (char *)malloc(st.st_size + 1); // 定义缓冲区读取文件的内容
    ssize_t n = read(fd2, file_buffer, st.st_size);
    if (n > 0)
    {
        file_buffer[n] = '\0';
        printf("%s", file_buffer);
    }
    free(file_buffer);
    close(fd2);
}

void printForOtherTerminal()
{
    int fd = open("/dev/pts/1", O_WRONLY | O_APPEND);
    if (fd < 0) return;

    const char *message = "hello hello hello!";
    write(fd, message, strlen(message));
}

void FILEandfd()
{
    // stdin、stdout、stderr都是FILE结构体的对象，里面都有描述fd的字段_fileno
    printf("stdin->fd: %d\n", stdin->_fileno);
    printf("stdout->fd: %d\n", stdout->_fileno);
    printf("stderr->fd: %d\n", stderr->_fileno);

    const char *message = "Jiasty!!!!!!!!!!!!!!!!!";

    // 显示器文件已经打开，fd为1，使用write向显示器写入
    fprintf(stdout, "1111111111111"); // fprintf是写入语言级缓冲区的，不刷新会在结束时自动刷新，所以会看到111111在write的后面
    fflush(stdout);                   // 手动刷新一下就会在前面了
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

    if (fd < 0)
        return; // 成功则返回大于等于0的fd
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
    Redirect();
    //GetFileInfo();
    // printForOtherTerminal();
    // FILEandfd();
    // systemCallFunc();
    //  testCfileFunc();

    return 0;
}