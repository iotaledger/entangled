#include <stdlib.h>
#include <string.h>

#include "common/curl-p/hashcash.h"
#include "common/trinary/trit_tryte.h"
#include "pow.h"

#define NONCE_LENGTH 27 * 3

trit_t* do_pow(Curl* const curl, trit_t const* const trits_in,
               size_t const trits_len, uint8_t const mwm) {
  tryte_t* nonce_trits = (tryte_t*)calloc(NONCE_LENGTH + 1, sizeof(tryte_t));

  curl_absorb(curl, trits_in, trits_len - HASH_LENGTH);
  memcpy(curl->state, trits_in + trits_len - HASH_LENGTH, HASH_LENGTH);

  // FIXME(th0br0) deal with result value of `hashcash` call
  hashcash(curl, BODY, HASH_LENGTH - NONCE_LENGTH, HASH_LENGTH, mwm);

  memcpy(nonce_trits, curl->state + HASH_LENGTH - NONCE_LENGTH, NONCE_LENGTH);

  return nonce_trits;
}
