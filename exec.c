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

static void connect(int fd1, int fd2);
static void do_exec(Cmd *cmd);
static void do_cd(const Vector *argv);
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

bool do_builtin(Cmd *cmd)
{
  if (strcmp(cmd->cmd, "exit") == 0) {
    exit(0);
  }
  if (strcmp(cmd->cmd, "chdir") == 0 || strcmp(cmd->cmd, "cd") == 0) {
    do_cd(cmd->argv);
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
        if (fd < 0) die("input redirection");
        connect(fd, STDIN_FILENO);
      }

      if (cmd->next) {
        // not tail
        close(fds2[0]);
        connect(fds2[1], STDOUT_FILENO);
      } else if (cmd->rd_out) {
        // tail
        int fd = open(cmd->rd_out, cmd->rd_out_flags, 0644);
        if (fd < 0) die("output redirection");
        connect(fd, STDOUT_FILENO);
      }

      argv = calloc(cmd->argv->size + 1, sizeof(char*));
      for (i = 0; i < cmd->argv->size; ++i) {
        argv[i] = (char*) vec_get(cmd->argv, i);
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

static void do_cd(const Vector *argv)
{
  char *dir;
  int argc = argv->size;
  if (argc == 0) die("illegal cd");
  if (argc == 1) {
    if (get_home(BUF)) {
      dir = BUF;
    }
  } else if (argc == 2) {
    dir = (char*) vec_get(argv, 1);
    if (*dir == '~' && get_home(BUF)) {
      strcat(BUF, dir+1);
      dir = BUF;
    }
  } else {
    char *cmd = (char*) vec_get(argv, 1);
    fprintf(stderr, "%s: too many arguments\n", cmd);
    return;
  }
  if (chdir(dir) < 0) die(dir);
}

static int get_home(char* buf)
{
  int len;
  strcpy(buf, getenv("HOME"));
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

  strcpy(BUF, getenv("PATH"));
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
