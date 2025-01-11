#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

//2.自定义退出码和退出信息
enum ExitCode
{
    Success = 0,
    Div_Zero,
    Mod_Zero
};
int exit_code = Success;
const char *CodeToErrString(int code)
{
    switch(code)
    {
        case Success:
            return "Success";
        case Div_Zero:
            return "div zero!";
        case Mod_Zero:
            return "mod zero!";
        default:
            return "unknow error!";
    }
}
int Div(int x, int y)
{
    if(y == 0)
    {
        exit_code = Div_Zero;
    }
    else
    {
        return x / y;
    }
}
void testDivZero()
{
    int ret = Div(20, 2);
    printf("ret:%d, errstr:%s\n", ret, CodeToErrString(exit_code));

    ret = Div(20, 0);
    printf("ret:%d, errstr:%s\n", ret, CodeToErrString(exit_code));
    if(exit_code != Success) exit(exit_code);
}

//1.展示系统默认退出码对应错误信息
void showExitCode()
{
    for(int i = 0; i <= 255; i++)
    {
        printf("exitCode:%d -> %s\n", i, strerror(i));
    }
}

int main()
{
    testDivZero();
    //showExitCode();

    return 0;
}