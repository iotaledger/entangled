#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/helpers/sign.h"
#include "common/kerl/kerl.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

IOTA_EXPORT char* iota_sign_address_gen(char const* const seed,
                                        size_t const index,
                                        size_t const security) {
  if (!(security > 0 && security <= 3)) return NULL;
  const size_t key_length = security * ISS_KEY_LENGTH;

  Kerl kerl;

  trit_t subseed[HASH_LENGTH];
  trit_t* key = calloc(key_length, sizeof(trit_t));

  char* address = calloc(1 + (HASH_LENGTH / RADIX), sizeof(tryte_t));

  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH / RADIX);
  iss_kerl_subseed(subseed, subseed, index, &kerl);

  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_key_digest(key, key, key_length, &kerl);
  iss_kerl_address(key, key, security * HASH_LENGTH, &kerl);

  kerl_reset(&kerl);

  trits_to_trytes(key, (tryte_t*)address, HASH_LENGTH);
  memset(key, 0, key_length * sizeof(trit_t));

  free(key);

  return address;
}

char* iota_sign_signature_gen(char const* const seed, size_t const index,
                              size_t const security,
                              char const* const bundleHash) {
  const size_t key_length = security * ISS_KEY_LENGTH;

  Kerl kerl;

  trit_t hash[HASH_LENGTH];
  trit_t subseed[HASH_LENGTH];
  trit_t* key = calloc(key_length, sizeof(trit_t));
  tryte_t* signature = calloc(1 + key_length / RADIX, sizeof(tryte_t));

  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH / RADIX);
  trytes_to_trits((tryte_t*)bundleHash, hash, HASH_LENGTH / RADIX);
  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_signature(key, hash, key, key_length, &kerl);
  trits_to_trytes(key, (tryte_t*)signature, key_length);
  memset(key, 0, key_length * sizeof(trit_t));

  free(key);

  return (char*)signature;
}

flex_trit_t* iota_flex_sign_address_gen(flex_trit_t const* const seed,
                                        size_t const index,
                                        size_t const security) {
  if (!(security > 0 && security <= 3)) return NULL;
  const size_t key_length = security * ISS_KEY_LENGTH;

  Kerl kerl;

  trit_t subseed[HASH_LENGTH];
  trit_t key[key_length];
  flex_trit_t* address =
      calloc(flex_trits_num_for_trits(HASH_LENGTH) + 1, sizeof(flex_trit_t));

  init_kerl(&kerl);

  flex_trit_array_to_int8(subseed, HASH_LENGTH, seed, HASH_LENGTH, HASH_LENGTH);
  iss_kerl_subseed(subseed, subseed, index, &kerl);

  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_key_digest(key, key, key_length, &kerl);
  iss_kerl_address(key, key, security * HASH_LENGTH, &kerl);

  kerl_reset(&kerl);

  int8_to_flex_trit_array(address, HASH_LENGTH, key, key_length, HASH_LENGTH);
  memset(key, 0, key_length * sizeof(trit_t));

  return address;
}

flex_trit_t* iota_flex_sign_signature_gen(flex_trit_t const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          flex_trit_t const* const bundleHash) {
  const size_t key_length = security * ISS_KEY_LENGTH;

  Kerl kerl;

  trit_t hash[HASH_LENGTH];
  trit_t subseed[HASH_LENGTH];
  trit_t key[key_length];
  flex_trit_t* signature =
      calloc(flex_trits_num_for_trits(key_length) + 1, sizeof(flex_trit_t));

  init_kerl(&kerl);

  flex_trit_array_to_int8(subseed, HASH_LENGTH, seed, HASH_LENGTH, HASH_LENGTH);
  flex_trit_array_to_int8(hash, HASH_LENGTH, bundleHash, HASH_LENGTH,
                          HASH_LENGTH);

  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_signature(key, hash, key, key_length, &kerl);
  int8_to_flex_trit_array(signature, key_length, key, key_length, key_length);
  memset(key, 0, key_length * sizeof(trit_t));

  return signature;
}

#undef RADIX
#undef NONCE_LENGTH
