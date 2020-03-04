#include <stdio.h>
#include <stdbool.h>
#include "38sh.h"

#define DEFAULT_BUF_SIZE 1024

bool term;

char BUF[DEFAULT_BUF_SIZE];

int main(int argc, char *argv[]) {
  if (argc == 1) {
    term = true;
    prompt(NULL);
  } else {
    term = false;
    int i;
    for (i = 1; i < argc; ++i) {
      prompt(argv[1]);
    }
  }
}

void print_header()
{
  printf("38sh$ ");
}

