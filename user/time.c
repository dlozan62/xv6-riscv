#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int status;
  int start, end;
  struct rusage rusage;

  if(argc < 2){
    fprintf(2, "Usage: time command [args...]\n");
    exit(1);
  }

  start = uptime();

  pid = fork();

  if(pid < 0){
    fprintf(2, "time: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    exec(argv[1], &argv[1]);

    fprintf(2, "time: exec failed\n");
    exit(1);
  }

  wait2(&status, &rusage);

  end = uptime();

  printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n",
         end - start,
         rusage.cputime,
         (rusage.cputime * 100) / (end - start));

  exit(0);
}
