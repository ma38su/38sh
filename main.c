#include <stdio.h>
#include "38sh.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    do_sh(NULL);
  } else {
    int i;
    for (i = 1; i < argc; ++i) {
      do_sh(argv[1]);
    }
  }
}

