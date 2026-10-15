#include "kernel/types.h"
#include "user/user.h"
#define WORK 1000000000

int
main(void)
{
  int release[2];
  int results[2];
  int pid;
  int i;
  int start_tick;
  int data[4];
  char token;

  if (pipe(release) < 0 || pipe(results) < 0) {
    printf("pipe failed\n");
    exit(1);
  }

  // Create two workers with different base priorities.
  for (i = 0; i < 2; i++) {
    pid = fork();

    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      int priority;
      int start;
      int finish;
      volatile uint n;

      close(release[1]);
      close(results[0]);

      priority = (i == 0) ? 0 : 49;

      if (setpriority(priority) < 0)
        exit(1);

      // Both workers wait until the parent releases them.
      if (read(release[0], &token, 1) != 1)
        exit(1);

      // Approximate first-run time after release.
      start = uptime();

      for (n = 0; n < WORK; n++)
        ;

      finish = uptime();

      data[0] = getpid();
      data[1] = priority;
      data[2] = start;
      data[3] = finish;

      write(results[1], data, sizeof(data));

      close(release[0]);
      close(results[1]);
      exit(0);
    }
  }

  close(release[0]);
  close(results[1]);

  // Record the release time, then release both workers.
  start_tick = uptime();
  write(release[1], "GO", 2);
  close(release[1]);

  printf("Release tick: %d\n", start_tick);
  printf("Waiting for worker results...\n");

  for (i = 0; i < 2; i++) {
    if (read(results[0], data, sizeof(data)) != sizeof(data)) {
      printf("Failed to read worker result\n");
      exit(1);
    }

    printf("PID %d priority %d: response=%d ticks, "
           "turnaround=%d ticks\n",
           data[0], data[1],
           data[2] - start_tick,
           data[3] - start_tick);
  }

  close(results[0]);
  wait(0);
  wait(0);

  printf("Aging test complete.\n");
  exit(0);
}

