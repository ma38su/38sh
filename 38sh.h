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
  Token *head;
  Token *token;
};

struct Cmd {
  pid_t pid;
  char* cmd;
  Vector* argv;
  Cmd *next;
  char* rd_in;
  char* rd_out;
  mode_t rd_out_flags;
  char* rd_err;
  mode_t rd_err_flags;
};

void print_header();
void do_cmd(Cmd *cmd);
void die(const char *fmt, ...);

void prompt(const char *path);

char *substr(char *str, int len);

TokenPtr *tokenize(char *p);
void free_token(TokenPtr *ptr);
char *token_to_s(Token *token);

extern bool term;
extern char BUF[];

#endif
