#include "../lib/ccurl.h"
#include "../lib/hash.h"
#include "../lib/pearl_diver.h"
#include "../lib/util/converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRYTE_LENGTH 2673

#define HINTS                                                                  \
  "### CCURL DIGEST ###\nUsage:\n\tccurl-cli [TRYTES (length: %d)] \n\techo "  \
  "TRYTES | ccurl-cli \n"

int main(int argc, char* argv[]) {
  char* buf;

  if (argc > 1 && strlen(argv[1]) >= TRYTE_LENGTH) {
    buf = argv[1];
    // memcpy(buf, argv[1], sizeof(char)*TRYTE_LENGTH);
  } else {
    fprintf(stderr, HINTS, TRYTE_LENGTH);
    return 1;
  }
  fputs(ccurl_digest_transaction(buf), stdout);
  return 0;
}
