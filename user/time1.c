#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int start, end;

  if(argc < 2){
    printf("Usage: time1 command [args...]\n");
    exit(1);
  }

  start = uptime();

  int pid = fork();

  if(pid < 0){
    printf("time1: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    exec(argv[1], &argv[1]);

    printf("time1: exec failed\n");
    exit(1);
  }

  wait(0);

  end = uptime();

  printf("Time: %d ticks elapsed time: %d ticks\n",
         end - start, end - start);

  exit(0);
}
