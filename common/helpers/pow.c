#include <stdlib.h>
#include <string.h>

#include "common/curl-p/hashcash.h"
#include "common/pow/pow.h"
#include "common/trinary/trit_tryte.h"
#include "pow.h"

#define NONCE_LENGTH 27 * 3

char* iota_pow(char const* const trytes_in, uint8_t const mwm) {
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
