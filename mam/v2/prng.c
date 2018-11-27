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

  trit_array_t input[5] = {{.trits = prng->key,
                            .num_trits = MAM2_PRNG_KEY_SIZE,
                            .num_bytes = MAM2_PRNG_KEY_FLEX_SIZE,
                            .dynamic = 0},
                           dest_array,
                           *nonce1,
                           *nonce2,
                           *nonce3};

  sponge_init(prng->sponge);
  sponge_absorbn_flex(prng->sponge, MAM2_SPONGE_CTL_KEY, 5, input);
  sponge_squeeze_flex(prng->sponge, MAM2_SPONGE_CTL_PRN, output);
}
