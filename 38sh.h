#ifndef SH38_H
#define SH38_H

#include <unistd.h>
#include "vector.h"

typedef struct Token Token;
typedef struct TokenPtr TokenPtr;
typedef struct Cmd Cmd;

typedef enum {
  TK_RESERVED,
  TK_TEXT,
  TK_STR,
  TK_EOF,
} TokenKind;

struct Token {
  TokenKind kind;
  Token *next;
  char *str;
  int len;
};

struct TokenPtr {
  Token *token;
};

struct Cmd {
  pid_t pid;
  char* cmd;
  Vector* argv;
  Cmd *next;
  char* redirect;
  mode_t redirect_flags;
};

void print_header();
void do_cmd(Cmd *cmd);
void die(const char *fmt, ...);

void prompt(const char *path);

char *substr(char *str, int len);

Token *tokenize(char *p);
char *token_to_s(Token *token);

extern bool term;
extern char BUF[];

#endif
