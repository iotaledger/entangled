#include "curl.h"
#include "util/converter.h"
#include <stdio.h>
#include <stdlib.h>

EXPORT char* ccurl_digest_transaction(char* trytes) {
  char* hash;
  curl_t curl;
  init_curl(&curl);
  size_t length = strnlen(trytes, TRANSACTION_LENGTH);
  char digest[HASH_LENGTH];
  char* input =
      trits_from_trytes(trytes, length < TRYTE_LENGTH ? length : TRYTE_LENGTH);
  absorb(&curl, input, length * 3);
  squeeze(&curl, digest, HASH_LENGTH);
  hash = trytes_from_trits(digest, 0, HASH_LENGTH);
  // hash[HASH_LENGTH] = 0;
  free((void*) input);
  return hash;
}
