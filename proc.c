#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "38sh.h"
#include "vector.h"

bool __debug = false;

void do_exec(const char *cmd, const Vector *argv) {
  int i, len, status;
  pid_t pid;
  char **argv_ary;

  len = argv->size + 1;
  argv_ary = calloc(len, sizeof(char*));
  for (i = 0; i < argv->size; ++i) {
    argv_ary[i] = (char*) vec_get(argv, i);
  }

  pid = fork();
  if (pid < 0) die("fork(2) failed");
  if (pid == 0) {
    execv(cmd, argv_ary);
    die(cmd);
  }

  waitpid(pid, &status, 0);

  if (__debug) {
    printf("cmd: %s\n", cmd);
    for (i = 0; i < len; ++i) {
      printf(" arg%d: %s\n", i, argv_ary[i]);
    }
  }
}

