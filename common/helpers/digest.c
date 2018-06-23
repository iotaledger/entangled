#include <string.h>

#include "common/curl-p/digest.h"
#include "common/trinary/trit_tryte.h"
#include "digest.h"

#define TRYTE_LENGTH 2673
#define TRANSACTION_LENGTH TRYTE_LENGTH * 3

char* iota_digest(char const* const trytes) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  size_t length = strnlen(trytes, TRANSACTION_LENGTH);
  trit_t input[sizeof(char) * length * 3];
  trytes_to_trits((tryte_t*)trytes, input, length);

  char trits_hash[HASH_LENGTH];
  curl_digest(input, length * 3, trits_hash, &curl);

  char* hash = calloc(HASH_LENGTH / 3 + 1, sizeof(char));
  trits_to_trytes((trit_t*)trits_hash, (tryte_t*)hash, HASH_LENGTH);

  return hash;
}
