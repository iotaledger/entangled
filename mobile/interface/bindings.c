#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/curl-p/hashcash.h"
#include "common/curl-p/trit.h"
#include "common/kerl/kerl.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_tryte.h"

#define NONCE_LENGTH 27 * 3
#define RADIX 3

char* do_pow(const char* trytes_in, int mwm) {
  Curl curl;

  int tryte_len = strlen(trytes_in);
  int trits_len = tryte_len * 3;

  tryte_t* nonce_trytes =
      (tryte_t*)calloc(NONCE_LENGTH / 3 + 1, sizeof(tryte_t));
  trit_t* trits = (trit_t*)malloc(sizeof(trit_t) * trits_len);

  trytes_to_trits((tryte_t*)trytes_in, trits, tryte_len);

  curl.type = CURL_P_81;
  init_curl(&curl);
  curl_absorb(&curl, trits, trits_len - HASH_LENGTH);
  memcpy(curl.state, trits + trits_len - HASH_LENGTH, HASH_LENGTH);

  free(trits);

  // FIXME(th0br0) deal with result value of `hashcash` call
  hashcash(&curl, BODY, HASH_LENGTH - NONCE_LENGTH, HASH_LENGTH, mwm);

  trits_to_trytes(curl.state + HASH_LENGTH - NONCE_LENGTH, nonce_trytes,
                  NONCE_LENGTH);

  return (char*)nonce_trytes;
}

char* generate_address(const char* seed, const size_t index,
                       const size_t security) {
  if (!(security > 0 && security <= 3)) return NULL;
  const size_t key_length = security * ISS_KEY_LENGTH;

  Kerl kerl;
  size_t i;

  trit_t subseed[HASH_LENGTH];
  trit_t key[key_length];
  char* address = calloc(1 + (HASH_LENGTH / RADIX), sizeof(tryte_t));

  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH / RADIX);
  iss_kerl_subseed(subseed, subseed, index, &kerl);

  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  for (i = 0; i < security; i++) {
    iss_kerl_key_digest(key + i * ISS_KEY_LENGTH, key + i * HASH_LENGTH,
                        ISS_KEY_LENGTH, &kerl);
  }

  iss_kerl_address(key, key, security * HASH_LENGTH, &kerl);
  kerl_reset(&kerl);

  trits_to_trytes(key, (tryte_t*)address, HASH_LENGTH);
  memset(key, 0, key_length * sizeof(trit_t));

  return address;
}
char* generate_signature(const char* seed, const size_t index,
                         const size_t security, const size_t bundleHash) {
  const size_t key_length = security * ISS_KEY_LENGTH;

  Kerl kerl;
  size_t i;

  trit_t subseed[HASH_LENGTH];
  trit_t key[key_length];

  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH / RADIX);
  iss_kerl_subseed(subseed, subseed, index, &kerl);

  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  for (i = 0; i < security; i++) {
    iss_kerl_key_digest(key + i * ISS_KEY_LENGTH, key + i * HASH_LENGTH,
                        ISS_KEY_LENGTH, &kerl);
  }


  return "";
}

#undef RADIX
#undef NONCE_LENGTH
