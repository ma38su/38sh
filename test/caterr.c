#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <msg-to-stderr>\n", argv[0]);
    return 1;
  }
  fprintf(stderr, "%s\n", argv[1]);
  return 0;
}
