#ifndef PROCSTATE_H
#define PROCSTATE_H

enum procstate {
  UNUSED,
  USED,
  SLEEPING,
  RUNNABLE,
  RUNNING,
  ZOMBIE
};

#endif
