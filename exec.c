#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "38sh.h"
#include "vector.h"

extern char **environ;

static void connect(int fd1, int fd2);
static void connect2(int fd1, int fd2, int fd3);
static void do_exec(Cmd *cmd);
static void do_cd(const char *cmd, const Vector *argv);
static void do_export(const Vector *argv);
static bool search_exec(char *cmd, char *buf);
static int get_home(char *buf);

static bool is_file(const char *path)
{
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

static void connect2(int fd1, int fd2, int fd3)
{
  close(fd2);
  dup2(fd1, fd2);

  close(fd3);
  dup2(fd1, fd3);

  close(fd1);
}

bool do_builtin(Cmd *cmd)
{
  if (strcmp(cmd->cmd, "exit") == 0) {
    exit(0);
  }
  if (strcmp(cmd->cmd, "export") == 0) {
    do_export(cmd->argv);
    return true;
  }
  if (strcmp(cmd->cmd, "chdir") == 0 || strcmp(cmd->cmd, "cd") == 0) {
    do_cd(cmd->cmd, cmd->argv);
    return true;
  }
  return false;
}

void do_cmd(Cmd *cmd)
{
  if (do_builtin(cmd)) {
    return;
  }
  do_exec(cmd);
}

static void do_exec(Cmd *cmd_head)
{
  int i, status;

  char cmd_path[512];
  char **argv;

  int fds1[2] = {-1, -1};
  int fds2[2] = {-1, -1};

  Cmd *cmd;
  cmd = cmd_head;
  while (cmd) {
    fds1[0] = fds2[0];
    fds1[1] = fds2[1];
    if (cmd->next) {
      if (pipe(fds2) < 0) die("pipe");
    }

    cmd->pid = fork();
    if (cmd->pid < 0) die("fork(2) failed");
    if (cmd->pid == 0) {
      if (!search_exec(cmd->cmd, cmd_path)) {
        fprintf(stderr, "%s: command not found\n", cmd->cmd);
        return;
      }

      if (cmd != cmd_head) {
        // not head
        connect(fds1[0], STDIN_FILENO);
        close(fds1[1]);
      } else if (cmd->rd_in) {
        // head
        int fd = open(cmd->rd_in, O_RDONLY);
        if (fd < 0) die("stdin redirection");
        connect(fd, STDIN_FILENO);
      }

      if (cmd->next) {
        // not tail
        close(fds2[0]);
        connect(fds2[1], STDOUT_FILENO);
      } else if (cmd->rd_out) {
        // tail
        int fd = open(cmd->rd_out, cmd->rd_out_flags, 0644);
        if (fd < 0) die("stdout redirection");
        if (cmd->rd_out != cmd->rd_err) {
          connect(fd, STDOUT_FILENO);
        } else {
          connect2(fd, STDOUT_FILENO, STDERR_FILENO);
        }
      }

      if (cmd->rd_err && cmd->rd_err != cmd->rd_out) {
        int fd = open(cmd->rd_err, cmd->rd_err_flags, 0644);
        if (fd < 0) die("stderr redirection");
        connect(fd, STDERR_FILENO);
      }

      argv = calloc(cmd->argv->size + 2, sizeof(char*));
      argv[0] = cmd->cmd;
      for (i = 0; i < cmd->argv->size; ++i) {
        argv[i+1] = (char*) vec_get(cmd->argv, i);
      }
      execv(cmd_path, argv);
      die("illegal exec: %s", cmd_path);
    }
    cmd = cmd->next;

    if (fds1[0] >= 0) close(fds1[0]);
    if (fds1[1] >= 0) close(fds1[1]);
  }

  cmd = cmd_head;
  while (cmd) {
    waitpid(cmd->pid, &status, 0);
    cmd = cmd->next;
  }
}

static void do_export(const Vector *argv)
{
  if (argv->size == 0) {
    char **p;
    for (p = environ; *p; p++) {
      printf("%s\n", *p);
    }
    // print all env vars
  } else {
    int i;
    for (i = 0; i < argv->size; ++i) {
      char *var = vec_get(argv, i);
      printf("var %s\n", var);
    }
  }
}

static void do_cd(const char *cmd, const Vector *argv)
{
  char *dir;
  int argc = argv->size;
  if (argc == 0) {
    if (get_home(BUF)) {
      dir = BUF;
    }
  } else if (argc == 1) {
    dir = (char*) vec_get(argv, 0);
    if (*dir == '~' && get_home(BUF)) {
      strcat(BUF, dir+1);
      dir = BUF;
    }
  } else {
    fprintf(stderr, "%s: too many arguments\n", cmd);
    return;
  }
  if (chdir(dir) < 0) die(dir);
}

static void get_env(char *key, char *buf) {
  char *val = getenv(key);
  if (val) {
    strcpy(buf, val);
  } else {
    buf[0] = '\0';
  }
}

static int get_home(char* buf)
{
  int len;
  get_env("HOME", buf);
  len = strlen(buf);
  while (len > 0 && buf[len - 1] == '/') {
    len--;
    buf[--len] = '\0';
  }
  return len;
}

static bool search_exec(char* cmd, char* cmd_path)
{
  char *path;
  if (strchr(cmd, '/')) {
    strcpy(cmd_path, cmd);
    return is_file(cmd);
  }

  get_env("PATH", BUF);
  if (strlen(BUF) == 0) {
    strcpy(cmd_path, cmd);
    return is_file(cmd);
  }

  path = strtok(BUF, ":");
  while (path) {
    strcpy(cmd_path, path);
    strcat(cmd_path, "/");
    strcat(cmd_path, cmd);
    if (is_file(cmd_path)) {
      return true;
    }
    path = strtok(NULL, ":");
  }
  return false;
}
