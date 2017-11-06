#include "../lib/ccurl.h"
#include "../lib/hash.h"
#include "../lib/pearl_diver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#include "Windows.h"
#include "Winsock2.h"
#pragma comment(lib, "Ws2_32.lib")
*/
#ifndef _WIN32
#include <sys/select.h>
#else
#define STDIN_FILENO 0
#endif

#define TRYTE_LENGTH 2673

#define HINTS                                                                  \
  "### CCURL ###\nUsage:\n\tccurl-cli <MinWeightMagnitude> [TRYTES (length: "  \
  "%d)] \n\techo TRYTES | ccurl-cli <MinWeightMagnitude>\n"

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
  char *buf, *out = NULL, *str;
  buf = (char*)calloc((TRYTE_LENGTH + 1) + 1, sizeof(char));
  long minWeightMagnitude;

  if (argc < 2) {
    fprintf(stderr, HINTS, TRYTE_LENGTH);
    return 1;
  }

  if (argc > 2) {
    if (strlen(argv[2]) >= TRYTE_LENGTH) {
      memcpy(buf, argv[2], sizeof(char) * strlen(argv[2]));
    } else {
      fprintf(stderr, HINTS, TRYTE_LENGTH);
      return 1;
    }
  }
  minWeightMagnitude = atol(argv[1]);
  if (minWeightMagnitude == 0) {
    fprintf(stderr, HINTS, TRYTE_LENGTH);
    return 1;
  }
  if ((str = getenv("CCURL_LOOP_COUNT"))) {
    ccurl_pow_set_loop_count(atol(str));
  }
  ccurl_pow_init();
  out = ccurl_pow(buf, minWeightMagnitude);
  if (out != NULL) {
    fputs(out, stdout);
  }
  free(out);
  free(buf);
  ccurl_pow_finalize();
  return 0;
}
