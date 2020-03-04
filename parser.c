#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "38sh.h"

#define DEFAULT_BUF_SIZE 1024

static char *read_line(FILE *f);

static char *expect_cmd(TokenPtr *tp);
static char *expect_arg(TokenPtr *tp);
static char *consume_arg(TokenPtr *tp);
static bool consume(TokenPtr *tp, char *op);

static Cmd *command(TokenPtr *tp);
static Cmd *parse(TokenPtr *tp);

static Cmd *parse(TokenPtr *tp)
{
  Cmd *cmd = command(tp);
  if (consume(tp, "<")) {
    cmd->rd_in = expect_arg(tp);
  }

  if (consume(tp, "|")) {
    cmd->next = parse(tp);
  } else if (consume(tp, ">")) {
    cmd->rd_out = expect_arg(tp);
    cmd->rd_out_flags = O_WRONLY | O_CREAT | O_TRUNC;
  } else if (consume(tp, ">>")) {
    cmd->rd_out = expect_arg(tp);
    cmd->rd_out_flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  return cmd;
}

static Cmd *command(TokenPtr *tp)
{
  Cmd *cmd;
  cmd = calloc(1, sizeof(Cmd));

  cmd->pid = 0;
  cmd->cmd = expect_cmd(tp);
  cmd->argv = new_vector();

  vec_add(cmd->argv, cmd->cmd);
  while (tp->token) {
    char *arg = consume_arg(tp);
    if (!arg) {
      break;
    }
    vec_add(cmd->argv, arg);
  }
  return cmd;
}

void prompt(const char *path)
{
  int i;
  FILE *f;
  TokenPtr *tp;

  if (path) {
    f = fopen(path, "r");
    if (!f) {
      die("path: %s", path);
    }
  } else {
    f = stdin;
  }

  Vector *cmds = new_vector();

  char *line;
  for (;;) {
    if (term) print_header();

    line = read_line(f);
    if (!line) break;

    tp = tokenize(line);
    if (!tp->token) {
      continue;
    }

    do {
      Cmd *cmd = parse(tp);
      vec_add(cmds, cmd);
    } while (consume(tp, "&&") || consume(tp, ";"));

    if (tp->token) {
      fprintf(stderr, "syntax error\n");
      continue;
    }

    for (i = 0; i < cmds->size; ++i) {
      Cmd *cmd = vec_get(cmds, i);
      do_cmd(cmd);
    }
    vec_clear(cmds);
    free_token(tp);
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

static bool consume(TokenPtr *tp, char *op) {
  if (tp->token && tp->token->kind == TK_RESERVED
      && strlen(op) == tp->token->len
      && memcmp(op, tp->token->str, tp->token->len) == 0) {

    tp->token = tp->token->next;
    return true;
  }
  return false;
}

static char *expect_arg(TokenPtr *tp) {
  char *arg = consume_arg(tp);
  if (!arg) {
    die("not arg");
  }
  return arg;
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
