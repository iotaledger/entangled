#include "../lib/util/converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HINTS                                                                  \
  "### TRYTES TO LONG ###\nUsage:\n\t trytes-to-long <Tryte string all caps> " \
  "\n"

int get_stdin(char* str, int len) {

  int i = 0;
  char chr;
  struct timeval timeout;
  fd_set readfds, savefds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  savefds = readfds;

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  if (select(1, &readfds, NULL, NULL, &timeout)) {
    while ((chr = getchar()) != EOF) {
      if (i > len)
        return -1;
      str[i++] = chr;
    }
  }
  readfds = savefds;
  // str[i] = 0;
  return i;
}

int main(int argc, char* argv[]) {
  char* buf;

  if (argc > 1) {
    buf = argv[1];
  } else {
    buf = malloc(27 * sizeof(char));
    if (get_stdin(buf, 27) != 0) {
      fprintf(stderr, HINTS);
      return 1;
    }
  }
  init_converter();
  size_t length = strlen(buf);
  char* input = trits_from_trytes(buf, length);
  char value = long_value(input, 0, length * 3);
  fprintf(stdout, "%hhd", value);
  return 0;
}
