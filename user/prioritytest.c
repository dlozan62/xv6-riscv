#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int i;
  int pid;
  int ready[2];
  int release[2];
  int psid;
  char token;
  char *psargs[] = {"ps", 0};

  printf("Initial priority: %d\n", getpriority());

  printf("Setting priority to 10...\n");
  if (setpriority(10) < 0) {
    printf("setpriority failed\n");
    exit(1);
  }

  printf("New priority: %d\n", getpriority());

  printf("Testing invalid priority...\n");
  if (setpriority(50) < 0)
    printf("Invalid priority correctly rejected\n");
  else
    printf("ERROR: invalid priority accepted\n");

  printf("Priority after invalid attempt: %d\n",
         getpriority());

  if (pipe(ready) < 0 || pipe(release) < 0) {
    printf("pipe failed\n");
    exit(1);
  }

  printf("\nCreating workers with different priorities...\n");

  for (i = 1; i <= 3; i++) {
    pid = fork();

    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      close(ready[0]);
      close(release[1]);

      if (setpriority(i * 10) < 0) {
        printf("setpriority failed\n");
        exit(1);
      }

      printf("Worker PID %d has priority %d\n",
             getpid(), getpriority());

      // Tell the parent this worker is ready.
      token = 'R';
      write(ready[1], &token, 1);

      // Stay alive until parent releases this one.
      read(release[0], &token, 1);

      close(ready[1]);
      close(release[0]);
      exit(0);
    }
  }

  // The parent no longer needs these pipe ends
  close(ready[1]);
  close(release[0]);

  // Wait until all three workers are ready
  for (i = 0; i < 3; i++) {
    if (read(ready[0], &token, 1) != 1) {
      printf("Failed waiting for worker\n");
      exit(1);
    }
  }

  printf("\nProcess table while workers are waiting:\n");

  psid = fork();
  if (psid < 0) {
    printf("fork for ps failed\n");
    exit(1);
  }

  if (psid == 0) {
    exec("ps", psargs);
    printf("exec ps failed\n");
    exit(1);
  }

  wait(0);

  // Release the workers
  for (i = 0; i < 3; i++) {
    token = 'G';
    write(release[1], &token, 1);
  }

  close(ready[0]);
  close(release[1]);

  // Reap the three worker processes
  for (i = 0; i < 3; i++)
    wait(0);

  printf("\nAll workers finished.\n");
  printf("Parent priority: %d\n", getpriority());

  exit(0);
}
