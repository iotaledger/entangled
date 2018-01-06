#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/trinary/trit_tryte.h"
#include "common/curl-p/hashcash.h"
#include "common/curl-p/trit.h"

#define NONCE_LENGTH 27 * 3
//#define NONCE_LENGTH HASH_LENGTH

char* do_pow(const char* trytes_in, int mwm) {
  Curl curl;

  int tryte_len = strlen(trytes_in);
  int trits_len = tryte_len * 3;

  tryte_t* nonce_trytes = (tryte_t*) calloc(NONCE_LENGTH / 3 + 1, sizeof(tryte_t));
  trit_t* trits = (trit_t*) malloc(sizeof(trit_t) * trits_len);

  trytes_to_trits((tryte_t*) trytes_in, trits, tryte_len);

  curl.type = CURL_P_81;
  init_curl(&curl);
  curl_absorb(&curl, trits, trits_len - HASH_LENGTH);
  memcpy(curl.state, trits + trits_len - HASH_LENGTH, HASH_LENGTH);

  free(trits);

  // FIXME(th0br0) deal with result value of `hashcash` call
  hashcash(&curl, BODY, HASH_LENGTH - NONCE_LENGTH, HASH_LENGTH, mwm);

  trits_to_trytes(curl.state + HASH_LENGTH - NONCE_LENGTH, nonce_trytes, NONCE_LENGTH);


  return (char*) nonce_trytes;
}
