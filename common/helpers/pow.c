/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/helpers/pow.h"

#include <stdlib.h>
#include <string.h>

#include "common/curl-p/hashcash.h"
#include "common/pow/pow.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

#define NONCE_LENGTH 27 * 3

IOTA_EXPORT char* iota_pow(char const* const trytes_in, uint8_t const mwm) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  int tryte_len = strlen(trytes_in);
  int trits_len = tryte_len * 3;

  trit_t* trits = (trit_t*)calloc(trits_len, sizeof(trit_t));
  if (!trits) {
    return NULL;
  }
  trytes_to_trits((tryte_t*)trytes_in, trits, tryte_len);

  trit_t* nonce_trits = do_pow(&curl, trits, trits_len, mwm);
  free(trits);
  if (!nonce_trits) {
    return NULL;
  }
  tryte_t* nonce_trytes =
      (tryte_t*)calloc(NONCE_LENGTH / 3 + 1, sizeof(tryte_t));
  if (!nonce_trytes) {
    free(nonce_trits);
    return NULL;
  }
  trits_to_trytes(nonce_trits, nonce_trytes, NONCE_LENGTH);
  free(nonce_trits);

  return (char*)nonce_trytes;
}

IOTA_EXPORT flex_trit_t* iota_flex_pow(flex_trit_t const* const flex_trits_in,
                                       size_t num_trits, uint8_t const mwm) {
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;

  trit_t* trits = (trit_t*)calloc(num_trits, sizeof(trit_t));
  if (!trits) {
    return NULL;
  }
  flex_trits_to_trits(trits, num_trits, flex_trits_in, num_trits, num_trits);
  trit_t* nonce_trits = do_pow(&curl, trits, num_trits, mwm);
  free(trits);
  if (!nonce_trits) {
    return NULL;
  }
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NONCE_LENGTH);
  flex_trit_t* nonce_flex_trits =
      (flex_trit_t*)calloc(flex_len, sizeof(flex_trit_t));
  if (!nonce_flex_trits) {
    free(nonce_trits);
    return NULL;
  }
  flex_trits_from_trits(nonce_flex_trits, NONCE_LENGTH, nonce_trits,
                        NONCE_LENGTH, NONCE_LENGTH);
  free(nonce_trits);

  return nonce_flex_trits;
}
