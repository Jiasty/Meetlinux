#include<stdio.h>
#include<unistd.h>

int main()
{
  printf("only me");
  sleep(3);
  fork();
  printf("add one");
  sleep(5);





 // pid_t id = getpid();
 // pid_t pid = getppid();

 // while(1)
 // {
 //   printf("hello,pid = %d,ppid = %d\n",id,pid);
 //   sleep(1000);
 // }

  return 0;
}
