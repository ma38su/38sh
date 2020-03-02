#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "38sh.h"

#define DEFAULT_BUF_SIZE 1024

static char *read_line(FILE *f);
static char *expect_cmd(TokenPtr *tp);
static char *consume_arg(TokenPtr *tp);

void do_cd(const Vector *argv) {
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

  printf("$ ");

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

      if (strcmp(cmd, "exit") == 0) {
        exit(0);
      } else if (strcmp(cmd, "chdir") == 0 || strcmp(cmd, "cd") == 0) {
        do_cd(argv);
      } else {
        do_exec(cmd, argv);
        if (!tp.token) {
          break;
        }
      }
      tp.token = tp.token->next;
      printf("$ ");
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

static char *read_line(FILE *f) {
  static char BUF[DEFAULT_BUF_SIZE];
  if (fgets(BUF, DEFAULT_BUF_SIZE, f) == NULL) {
    return NULL;
  }
  return BUF;
}

char *substr(char *str, int len) {
  char *sub = calloc(len + 1, sizeof(char));
  strncpy(sub, str, len);
  return sub;
}
