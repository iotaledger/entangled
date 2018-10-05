/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "common/curl-p/digest.h"
#include "common/defs.h"
#include "common/helpers/digest.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

#define TRYTE_LENGTH 2673

IOTA_EXPORT char* iota_digest(const char* trytes) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  trit_t trits_hash[HASH_LENGTH_TRIT];
  size_t length = strnlen(trytes, TRYTE_LENGTH);
  trit_t* trits = calloc(length * RADIX, sizeof(trit_t));
  if (!trits) {
    return NULL;
  }
  trytes_to_trits((tryte_t*)trytes, trits, length);
  curl_digest(trits, length * 3, trits_hash, &curl);
  free(trits);

  char* hash = calloc(HASH_LENGTH_TRYTE + 1, sizeof(trit_t));
  if (!hash) {
    return NULL;
  }
  trits_to_trytes((trit_t*)trits_hash, (tryte_t*)hash, HASH_LENGTH_TRIT);

  return hash;
}

IOTA_EXPORT flex_trit_t* iota_flex_digest(flex_trit_t const* const flex_trits,
                                          size_t num_trits) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  trit_t trits_hash[HASH_LENGTH_TRIT];
  trit_t* trits = (trit_t*)calloc(num_trits, sizeof(trit_t));
  if (!trits) {
    return NULL;
  }
  flex_trits_to_trits(trits, num_trits, flex_trits, num_trits, num_trits);
  curl_digest(trits, num_trits, trits_hash, &curl);
  free(trits);

  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(num_trits);
  flex_trit_t* hash_flex_trits =
      (flex_trit_t*)calloc(flex_len, sizeof(flex_trit_t));
  if (!hash_flex_trits) {
    return NULL;
  }
  flex_trits_from_trits(hash_flex_trits, HASH_LENGTH_TRIT, trits_hash,
                        HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  return hash_flex_trits;
}
