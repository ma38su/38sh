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
static char *consume_arg(TokenPtr *tp);
static bool consume(TokenPtr *tp, char *op);

static Stmt *stmt(TokenPtr *tp);
static Stmt *parse(TokenPtr *tp);

static void print_header();

static Stmt *parse(TokenPtr *tp)
{
  Stmt *st = stmt(tp);
  if (consume(tp, "|")) {
    st->next = parse(tp);
  }
  return st;
}

static Stmt *stmt(TokenPtr *tp)
{
  Stmt *st;
  st = calloc(1, sizeof(Stmt));

  st->cmd = expect_cmd(tp);
  st->argv = new_vector();
  st->in = -1;
  st->out = -1;

  vec_add(st->argv, st->cmd);
  while (tp->token) {
    char *arg = consume_arg(tp);
    if (!arg) {
      break;
    }
    vec_add(st->argv, arg);
  }
  return st;
}

void do_sh(const char *path)
{
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

  print_header();

  char *line;
  while ((line = read_line(f)) != NULL) {
    tp.token = tokenize(line);
    Stmt *stmt = parse(&tp);
    while (stmt) {
      do_stmt(stmt);
      stmt = stmt->next;
    }
    print_header();
  }
  fclose(f);
}

static void print_header() {
  printf("38sh$ ");
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
