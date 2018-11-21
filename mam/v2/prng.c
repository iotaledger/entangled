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

static inline void prng_absorbn(isponge *const s, size_t const n,
                                trit_array_t const *const trits) {
  sponge_init(s);
  sponge_absorbn_flex(s, MAM2_SPONGE_CTL_KEY, n, trits);
}

// TODO switch output to trit_array_t
static inline void prng_squeeze(isponge *const s, trits_t output) {
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, output);
}

static inline trit_array_t trit_array_from_key(prng_t *const prng) {
  trit_array_t key = {.trits = prng->key,
                      .num_trits = MAM2_PRNG_KEY_SIZE,
                      .num_bytes = MAM2_PRNG_KEY_FLEX_SIZE,
                      .dynamic = 0};
  return key;
}

static void trit_array_set_dest_tryte(trit_array_t *const trit_array,
                                      uint8_t const dest) {
  trit_t trits[3];

  long_to_trits(dest, trits);
  flex_trits_from_trits(trit_array->trits, 3, trits, 3, 3);
}

/*
 * Public functions
 */

void prng_init(prng_t *const prng, isponge *const sponge,
               flex_trit_t const *const key) {
  prng->sponge = sponge;
  memcpy(prng->key, key, MAM2_PRNG_KEY_FLEX_SIZE);
}

void prng_reset(prng_t *const prng) {
  prng->sponge = NULL;
  memset(prng->key, FLEX_TRIT_NULL_VALUE, MAM2_PRNG_KEY_FLEX_SIZE);
}

// TODO switch output to trit_array_t
void prng_gen(prng_t *const prng, uint8_t const dest,
              trit_array_t const *const nonce, trits_t output) {
  trit_array_t null = trit_array_null();
  prng_gen3(prng, dest, nonce, &null, &null, output);
}

// TODO switch output to trit_array_t
void prng_gen2(prng_t *const prng, uint8_t const dest,
               trit_array_t const *const nonce1,
               trit_array_t const *const nonce2, trits_t output) {
  trit_array_t null = trit_array_null();
  prng_gen3(prng, dest, nonce1, nonce2, &null, output);
}

// TODO switch output to trit_array_t
void prng_gen3(prng_t *const prng, uint8_t const dest,
               trit_array_t const *const nonce1,
               trit_array_t const *const nonce2,
               trit_array_t const *const nonce3, trits_t output) {
  TRIT_ARRAY_DECLARE(dest_trits, 3);
  trit_array_set_dest_tryte(&dest_trits, dest);

  trit_array_t trits_to_absorb[5] = {trit_array_from_key(prng), dest_trits,
                                     *nonce1, *nonce2, *nonce3};

  prng_absorbn(prng->sponge, 5, (trit_array_t *)trits_to_absorb);
  prng_squeeze(prng->sponge, output);
}
