/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/prng.h"
#include "common/trinary/trit_long.h"

/*
 * Private functions
 */

static inline void prng_absorbn(sponge_t *const s, size_t const n,
                                trit_array_t const *const input) {
  sponge_init(s);
  sponge_absorbn_flex(s, MAM2_SPONGE_CTL_KEY, n, input);
}

static inline void prng_squeeze(sponge_t *const s, trit_array_t *const output) {
  sponge_squeeze_flex(s, MAM2_SPONGE_CTL_PRN, output);
}

static inline trit_array_t prng_key_trit_array(prng_t *const prng) {
  trit_array_t key = {.trits = prng->key,
                      .num_trits = MAM2_PRNG_KEY_SIZE,
                      .num_bytes = MAM2_PRNG_KEY_FLEX_SIZE,
                      .dynamic = 0};
  return key;
}

/*
 * Public functions
 */

void prng_init(prng_t *const prng, sponge_t *const sponge,
               flex_trit_t const *const key) {
  prng->sponge = sponge;
  memcpy(prng->key, key, MAM2_PRNG_KEY_FLEX_SIZE);
}

void prng_reset(prng_t *const prng) {
  prng->sponge = NULL;
  memset(prng->key, FLEX_TRIT_NULL_VALUE, MAM2_PRNG_KEY_FLEX_SIZE);
}

void prng_gen(prng_t *const prng, uint8_t const dest,
              trit_array_t const *const nonce, trit_array_t *const output) {
  trit_array_t null_trits = trit_array_null();
  prng_gen3(prng, dest, nonce, &null_trits, &null_trits, output);
}

void prng_gen2(prng_t *const prng, uint8_t const dest,
               trit_array_t const *const nonce1,
               trit_array_t const *const nonce2, trit_array_t *const output) {
  trit_array_t null_trits = trit_array_null();
  prng_gen3(prng, dest, nonce1, nonce2, &null_trits, output);
}

void prng_gen3(prng_t *const prng, uint8_t const dest,
               trit_array_t const *const nonce1,
               trit_array_t const *const nonce2,
               trit_array_t const *const nonce3, trit_array_t *const output) {
  TRIT_ARRAY_DECLARE(dest_array, 3);
  trit_t dest_trits[3];

  long_to_trits(dest, dest_trits);
  flex_trits_from_trits(dest_array.trits, 3, dest_trits, 3, 3);

  trit_array_t input[5] = {prng_key_trit_array(prng), dest_array, *nonce1,
                           *nonce2, *nonce3};

  prng_absorbn(prng->sponge, 5, (trit_array_t *)input);
  prng_squeeze(prng->sponge, output);
}
