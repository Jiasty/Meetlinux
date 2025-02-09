#include"mystdio.h"


myFILE* my_fopen(const char* path, const char* mode)
{
    int flags = 0; //文件打开方式
    int iscreate = 0; //是否需要创建文件标志
    mode_t modes = 0666; //文件默认权限

    if(strcmp(mode, "r") == 0)
    {
        flags = (O_RDONLY);
        iscreate = 1;
    }
    else if(strcmp(mode, "w") == 0)
    {
        flags = (O_WRONLY | O_CREAT | O_TRUNC);
        iscreate = 1;
    }
    else if(strcmp(mode, "a") == 0)
    {
        flags = (O_WRONLY | O_CREAT | O_APPEND);
        iscreate = 1;
    }

    int fd = 0;
    if(iscreate) fd = open(path, flags, modes);
    else fd = open(path, flags);
    if(fd < 0) return NULL;

    myFILE* fp = (myFILE*)malloc(sizeof(myFILE));
    if(fp == NULL) return NULL;

    fp->fileno = fd;
    fp->flag = FLUSH_LINE;
    fp->capacity = CACHESIZE;
    fp->pos = 0;

    return fp;
}

ssize_t my_fwrite (const char* ptr, size_t count, myFILE* fp)
{
    //写入操作本质是拷贝，如果条件允许，就刷新
    memcpy(fp->cache + fp->pos, ptr, count); //要考虑越界，扩容
    fp->pos += count;

    if((fp->flag & FLUSH_LINE) && fp->cache[fp->pos - 1] == '\n') //此处只实现了行刷新
    {
        my_fflush(fp);
    }

    return count;
}

void my_fflush(myFILE* fp)
{
    write(fp->fileno, fp->cache, fp->pos);
    fp->pos = 0;
}

void my_fclose(myFILE* fp)
{
    my_fflush(fp);
    close(fp->fileno);
    free(fp);
}
