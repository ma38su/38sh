#ifndef SH38_H
#define SH38_H

#include "vector.h"

typedef struct Token Token;
typedef struct TokenPtr TokenPtr;
typedef struct Stmt Stmt;

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

struct Stmt {
  char* cmd;
  Vector* argv;
  Stmt *next;

  int in;
  int out;
};

void do_stmt(Stmt *stmt);
void die(const char *fmt, ...);

void do_sh(const char *path);

char *substr(char *str, int len);

Token *tokenize(char *p);
char *token_to_s(Token *token);

#endif
