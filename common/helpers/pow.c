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

  trit_t* trits = (trit_t*)malloc(sizeof(trit_t) * trits_len);

  trytes_to_trits((tryte_t*)trytes_in, trits, tryte_len);
  tryte_t* nonce_trytes =
      (tryte_t*)calloc(NONCE_LENGTH / 3 + 1, sizeof(tryte_t));

  trit_t* nonce_trits = do_pow(&curl, trits, trits_len, mwm);
  free(trits);

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
  flex_trit_array_to_int8(trits, num_trits, flex_trits_in, num_trits,
                          num_trits);
  trit_t* nonce_trits = do_pow(&curl, trits, num_trits, mwm);
  free(trits);
  size_t flex_len = flex_trits_num_for_trits(NONCE_LENGTH);
  flex_trit_t* nonce_flex_trits =
      (flex_trit_t*)calloc(flex_len, sizeof(flex_trit_t));
  int8_to_flex_trit_array(nonce_flex_trits, NONCE_LENGTH, nonce_trits,
                          NONCE_LENGTH, NONCE_LENGTH);
  free(nonce_trits);

  return nonce_flex_trits;
}
