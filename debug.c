#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

void die(const char *fmt, ...) {
  perror(fmt);

  fprintf(stderr, "ERR: ");

  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

