#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "38sh.h"

static int text_len(char *p0);
static Token *new_token(TokenKind kind, Token *cur, char *str);

static char *next_ptr(char *p0, char c)
{
  char *p = p0;
  while (*p != c) {
    p++;
  }
  return p;
}

void free_token(TokenPtr *ptr)
{
  Token *token;
  Token *tmp;
  token = ptr->head;
  while (token) {
    tmp = token;
    token = token->next;
    free(tmp);
  }
  free(ptr);
}

TokenPtr *tokenize(char *p)
{

  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // skip blank
    if (isspace(*p)) {
      p++;
      continue;
    }

    // string literal
    if (*p == '\'') {
      p++;

      cur = new_token(TK_STR, cur, p);

      char *p0 = p;
      p = next_ptr(p, '\'');
      cur->len = p - p0;

      p++;
      continue;
    }
    // string literal
    if (*p == '"') {
      p++;

      cur = new_token(TK_TEXT, cur, p);

      char *p0 = p;
      p = next_ptr(p, '"');
      cur->len = p - p0;

      p++;
      continue;
    }

    if (memcmp(p, "&>>", 3) == 0) {
      // stdout & stderr > file
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 3;
      p += 3;
      continue;
    }
    if (memcmp(p, ">&2", 3) == 0) {
      // stdout > stderr
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 3;
      p += 3;
      continue;
    }
    if (memcmp(p, "2>>", 3) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 3;
      p += 3;
      continue;
    }

    if (memcmp(p, ">>", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }
    if (memcmp(p, "2>", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }
    if (memcmp(p, "&>", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }
    if (memcmp(p, "&&", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }

    if (strchr("|&><;", *p)) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    int l = text_len(p);
    if (l > 0) {
      cur = new_token(TK_TEXT, cur, p);
      p += l;
      cur->len = l;
      continue;
    }
    die("unexpected token");
  }

  TokenPtr *ptr;
  ptr = calloc(1, sizeof(TokenPtr));

  ptr->head = head.next;
  ptr->token = head.next;
  return ptr;
}

static Token *new_token(TokenKind kind, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

static int text_len(char *p0)
{
  char *p = p0;
  while (!strchr("|&><;", *p) && !isspace(*p)) {
    p++;
  }
  return p - p0;
}

char *token_to_s(Token *token) {
  return substr(token->str, token->len); 
}

