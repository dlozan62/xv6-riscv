#ifndef PSTAT_H
#define PSTAT_H
#include "kernel/types.h"
#include "kernel/procstate.h"

struct rusage {
  uint cputime;
};

struct pstat {
  int pid;
  enum procstate state;
  int priority;
  uint readytime;
  uint64 size;
  int ppid;
  char name[16];
};

#endif
