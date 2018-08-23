#include "common/helpers/digest.h"

#include <stdlib.h>
#include <string.h>

#include "common/curl-p/digest.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

#define TRYTE_LENGTH 2673

IOTA_EXPORT char* iota_digest(const char* trytes) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  size_t length = strnlen(trytes, TRYTE_LENGTH);
  trit_t* input = calloc(length * RADIX, sizeof(trit_t));

  trytes_to_trits((tryte_t*)trytes, input, length);

  trit_t trits_hash[HASH_LENGTH];
  curl_digest(input, length * 3, trits_hash, &curl);

  free(input);

  char* hash = calloc(HASH_LENGTH / 3 + 1, sizeof(trit_t));
  trits_to_trytes((trit_t*)trits_hash, (tryte_t*)hash, HASH_LENGTH);

  return hash;
}

IOTA_EXPORT flex_trit_t* iota_flex_digest(flex_trit_t const* const flex_trits,
                                          size_t num_trits) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  trit_t* trits = (trit_t*)calloc(num_trits, sizeof(trit_t));
  flex_trit_array_to_int8(trits, num_trits, flex_trits, num_trits, num_trits);
  trit_t trits_hash[HASH_LENGTH];
  curl_digest(trits, num_trits, trits_hash, &curl);
  free(trits);
  size_t flex_len = flex_trits_num_for_trits(num_trits);
  flex_trit_t* hash_flex_trits =
      (flex_trit_t*)calloc(flex_len, sizeof(flex_trit_t));
  int8_to_flex_trit_array(hash_flex_trits, num_trits, trits_hash, num_trits,
                          num_trits);

  return hash_flex_trits;
}
