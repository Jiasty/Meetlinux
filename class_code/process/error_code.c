#include<stdio.h>
#include<string.h>
#include<unistd.h>


int main()
{
  for(int error = 0; error < 135; error++)
  {
    printf("error code:%d, %s\n",error, strerror(error));
  }
  return 0;
}
