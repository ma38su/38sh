#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "38sh.h"
#include "vector.h"

static void connect(int fd1, int fd2);
static void do_exec(Stmt *stmt);
static void do_cd(const Vector *argv);
static char* get_exec(char* cmd);

bool __debug = false;

static bool is_file(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return false;
  }
  return S_ISREG(st.st_mode) != 0;
}

static void connect(int fd1, int fd2)
{
  close(fd2);
  dup2(fd1, fd2);
  close(fd1);
}

void do_stmt(Stmt *stmt)
{
  if (strcmp(stmt->cmd, "exit") == 0) {
    exit(0);
  } else if (strcmp(stmt->cmd, "chdir") == 0 || strcmp(stmt->cmd, "cd") == 0) {
    do_cd(stmt->argv);
  } else {
    do_exec(stmt);
  }
}

static void do_exec(Stmt *stmt)
{
  int i, len, status;
  pid_t pid;

  Vector *vargv;

  char *cmd;
  char **argv;

  vargv = stmt->argv;
  len = vargv->size + 1;

  cmd = get_exec(stmt->cmd);
  if (!cmd) {
    printf("%s is not found\n", stmt->cmd);
  }

  argv = calloc(len, sizeof(char*));
  for (i = 0; i < vargv->size; ++i) {
    argv[i] = (char*) vec_get(vargv, i);
  }

  pid = fork();
  if (pid < 0) die("fork(2) failed");
  if (pid == 0) {
    //printf("fork %s\n", stmt->cmd);
    if (stmt->in >= 0) {
      //printf("%s in\n", stmt->cmd);
      connect(stmt->in, STDIN_FILENO);
    }
    if (stmt->next) {
      int fds[2];
      if (pipe(fds) < 0) {
        perror(stmt->cmd);
      }
      stmt->next->in = fds[0];

      stmt->out = fds[1];
      //printf("%s out\n", stmt->cmd);
      connect(stmt->out, STDOUT_FILENO);
      do_exec(stmt->next);
    }

    execv(cmd, argv);
    die(cmd);
  }

  //printf("wait for %s\n", stmt->cmd);
  waitpid(pid, &status, 0);

  if (__debug) {
    printf("cmd: %s\n", cmd);
    for (i = 0; i < len; ++i) {
      printf(" arg%d: %s\n", i, argv[i]);
    }
  }
}

static void do_cd(const Vector *argv)
{
  char *dir;
  int argc = argv->size;
  if (argc == 0) die("illegal cd");
  if (argc == 1) {
    dir = "~/";
  } else if (argc == 2) {
    dir = (char*) vec_get(argv, 1);
  } else {
    dir = (char*) vec_get(argv, 1);
    printf("cd: string not in pwd: %s\n", vec_get(argv, 1));
    return;
  }
  if (chdir(dir) < 0) die(dir);
}

static char* get_exec(char* cmd)
{
  char *env_paths;
  char paths[1024];
  char *path;

  if (strchr("./", *cmd)) {
    return cmd;
  }

  env_paths = getenv("PATH");
  if (!env_paths) {
    return cmd;
  }

  strcpy(paths, env_paths);
  path = strtok(paths, ":");

  while (path) {
    int buf_len = strlen(path) + 1 + strlen(cmd) + 1;
    char *buf = calloc(buf_len, sizeof(char));
    strcpy(buf, path);
    strcat(buf, "/");
    strcat(buf, cmd);
    if (is_file(buf)) {
      return buf;
    }
    free(buf);
    path = strtok(NULL, ":");
  }
  return NULL;
}
