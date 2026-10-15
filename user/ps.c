#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  struct pstat uproc[NPROC];
  int nprocs;
  int i;
  char *state;
  uint now;
  uint age;

  static char *states[] = {
    [SLEEPING]  "sleeping",
    [RUNNABLE]  "runnable",
    [RUNNING]   "running ",
    [ZOMBIE]    "zombie  "
  };

  nprocs = getprocs(uproc);
  if (nprocs < 0)
    exit(-1);

  // Get the current tick count.
  now = uptime();

  printf("pid\tstate\t\tpriority\tsize\tage\tppid\tname\n");

  for (i = 0; i < nprocs; i++) {
    state = states[uproc[i].state];
    age = 0;

    // Calculate age only for runnable processes.
    if (uproc[i].state == RUNNABLE) {
      age = now - uproc[i].readytime;
    }

    printf("%d\t%s\t%d\t\t%ld\t%d\t%d\t%s\n",
           uproc[i].pid,
           state,
           uproc[i].priority,
           uproc[i].size,
           age,
           uproc[i].ppid,
           uproc[i].name);
  }

  exit(0);
}

