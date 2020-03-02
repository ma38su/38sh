#ifndef SH38_H
#define SH38_H

typedef struct Token Token;
typedef struct TokenPtr TokenPtr;

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

void die(const char *fmt, ...);

char *substr(char *str, int len);

Token *tokenize(char *p);
char *token_to_s(Token *token);

#endif
