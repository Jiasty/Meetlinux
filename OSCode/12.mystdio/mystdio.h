#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define CACHESIZE 1024
#define FLUSH_NOW 1
#define FLUSH_LINE 2
#define FLUSH_FULL 4

typedef struct
{
    //属性
    unsigned int flag;
    int fileno;
    //缓冲区
    char cache[CACHESIZE];
    int capacity;
    int pos; //下次写入的位置

} myFILE;

myFILE* my_fopen(const char* path, const char* mode);
ssize_t my_fwrite (const char* ptr, size_t count, myFILE* fp); //自己定制的专写char类型数据
void my_fclose(myFILE* fp);
void my_fflush(myFILE* fp);