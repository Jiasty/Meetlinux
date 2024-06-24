#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>

int g_val = 0;

int main()
{
  pid_t id = fork();
  if(id < 0)
  {
    perror("fork");
    return 0;
  }
  else if(id == 0)
  { //child
    g_val = 100;
    printf("child[%d]: %d : %p\n", getpid(), g_val, &g_val);
  }
  else
  { //parent
    printf("parent[%d]: %d : %p\n", getpid(), g_val, &g_val);
  }
  sleep(1);
  return 0;


 // pid_t id = fork();
 // if(id == 0)
 // {
 //   //child process
 //   printf("I am child pid:%d ppid:%d\n",getpid(),getppid());
 // }
 // else
 // {
 //   //father process
 //   printf("I am parent pid:%d\n",getpid());
 // }
 // return 0;
}
