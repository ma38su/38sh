#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "38sh.h"
#include "vector.h"

#define DEFAULT_BUF_SIZE 1024;

static char *read_line(FILE *f);

static char *expect_cmd(TokenPtr *tp);
static char *consume_arg(TokenPtr *tp);

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

void do_sh(const char *path) {
  FILE *f;
  TokenPtr tp;

  if (path) {
    f = fopen(path, "r");
    if (!f) {
      die(path);
    }
  } else {
    f = stdin;
  }

  char *line;
  while ((line = read_line(f)) != NULL) {
    tp.token = tokenize(line);

    while (tp.token) {
      char *cmd = expect_cmd(&tp);
      Vector *argv = new_vector();
      vec_add(argv, cmd);
      while (tp.token) {
        char *arg = consume_arg(&tp);
        if (!arg) {
          break;
        }
        vec_add(argv, arg);
      }

      do_exec(cmd, argv);
      if (!tp.token) {
        break;
      }
      tp.token = tp.token->next;
    }
  }
  fclose(f);
}

static char *expect_cmd(TokenPtr *tp) {
  if (tp->token->kind != TK_TEXT) {
    die("not cmd");
  }
  char *cmd = substr(tp->token->str, tp->token->len);
  tp->token = tp->token->next;
  return cmd;
}

static char *consume_arg(TokenPtr *tp) {
  if (tp->token->kind != TK_TEXT && tp->token->kind != TK_STR) {
    return NULL;
  }
  char *arg = substr(tp->token->str, tp->token->len);
  tp->token = tp->token->next;
  return arg;
}

int main(int argc, char *argv[]) {
  FILE *f;
  if (argc == 1) {
    do_sh(NULL);
    f = stdin;
  } else {
    int i;
    for (i = 1; i < argc; ++i) {
      do_sh(argv[1]);
    }
  }
}

static char *read_line(FILE *f) {
  static char BUF[1024];
  if (fgets(BUF, 1024, f) == NULL) {
    return NULL;
  }
  return BUF;
}

char *substr(char *str, int len) {
  char *sub = calloc(len + 1, sizeof(char));
  strncpy(sub, str, len);
  return sub;
}

void die(const char *fmt, ...) {
  perror(fmt);

  fprintf(stderr, "ERR: ");

  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

