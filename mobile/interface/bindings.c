#include <stdlib.h>
#include <string.h>

#include "common/trinary/trit_tryte.h"
#include "common/curl-p/hashcash.h"
#include "common/curl-p/trit.h"

#define NONCE_LENGTH 27 * 3


char* do_pow(char* trytes_in, int mwm) {
  Curl curl;

  int tryte_len = strlen(trytes_in);
  int trits_len = tryte_len * 3;

  trit_t nonce_trits[NONCE_LENGTH];
  tryte_t* nonce_trytes = (tryte_t*) malloc(sizeof(tryte_t) * (NONCE_LENGTH / 3));
  trit_t* trits = (trit_t*) malloc(sizeof(trit_t) * trits_len);

  trytes_to_trits((tryte_t*) trytes_in, trits, trits_len);

  curl.type = CURL_P_81;
  init_curl(&curl);
  curl_absorb(&curl, trits, trits_len);

  // FIXME(th0br0) deal with result value of `hashcash` call
  hashcash(&curl, BODY, 0, trits_len, mwm);

  curl_squeeze(&curl, nonce_trits, NONCE_LENGTH);

  trits_to_trytes(nonce_trits, nonce_trytes, NONCE_LENGTH / 3);

  free(trits);

  return (char*) nonce_trytes;
}
