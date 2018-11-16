/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/trinary/trit_long.h"
#include "mam/v2/prng.h"

/*
 * Private functions
 */

static void prng_absorbn(isponge *const s, size_t const n,
                         trits_t const *const trits_to_absorb) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_KEY, n, trits_to_absorb);
}

static void prng_squeeze(isponge *const s, trits_t output) {
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, output);
}

/*
 * Public functions
 */

void prng_create(prng_t *const prng) {
  prng->sponge = NULL;
  memset(prng->key, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
}

void prng_init(prng_t *const prng, isponge *const sponge,
               flex_trit_t const *const key) {
  prng->sponge = sponge;
  memcpy(prng->key, key, FLEX_TRIT_SIZE_243);
}

// TODO Switch input/output to flex_trit_t when sponge handle them
void prng_gen(prng_t *const prng, uint8_t const dest, trits_t const nonce,
              trits_t output) {
  trit_t key_trits[MAM2_PRNG_KEY_SIZE];
  trit_t dest_trits[3];
  trits_t trits_to_absorb[3] = {trits_from_rep(MAM2_PRNG_KEY_SIZE, key_trits),
                                trits_from_rep(3, dest_trits), nonce};

  flex_trits_to_trits(key_trits, MAM2_PRNG_KEY_SIZE, prng->key,
                      MAM2_PRNG_KEY_SIZE, MAM2_PRNG_KEY_SIZE);
  long_to_trits(dest, dest_trits);

  prng_absorbn(prng->sponge, 3, trits_to_absorb);
  prng_squeeze(prng->sponge, output);
}

// TODO Switch input/output to flex_trit_t when sponge handle them
void prng_gen2(prng_t *const prng, uint8_t const dest, trits_t const nonce1,
               trits_t const nonce2, trits_t output) {
  trit_t key_trits[MAM2_PRNG_KEY_SIZE];
  trit_t dest_trits[3];
  trits_t trits_to_absorb[4] = {trits_from_rep(MAM2_PRNG_KEY_SIZE, key_trits),
                                trits_from_rep(3, dest_trits), nonce1, nonce2};

  flex_trits_to_trits(key_trits, MAM2_PRNG_KEY_SIZE, prng->key,
                      MAM2_PRNG_KEY_SIZE, MAM2_PRNG_KEY_SIZE);
  long_to_trits(dest, dest_trits);

  prng_absorbn(prng->sponge, 4, trits_to_absorb);
  prng_squeeze(prng->sponge, output);
}

// TODO Switch input/output to flex_trit_t when sponge handle them
void prng_gen3(prng_t *const prng, uint8_t const dest, trits_t const nonce1,
               trits_t const nonce2, trits_t const nonce3, trits_t output) {
  trit_t key_trits[MAM2_PRNG_KEY_SIZE];
  trit_t dest_trits[3];
  trits_t trits_to_absorb[5] = {trits_from_rep(MAM2_PRNG_KEY_SIZE, key_trits),
                                trits_from_rep(3, dest_trits), nonce1, nonce2,
                                nonce3};

  flex_trits_to_trits(key_trits, MAM2_PRNG_KEY_SIZE, prng->key,
                      MAM2_PRNG_KEY_SIZE, MAM2_PRNG_KEY_SIZE);
  long_to_trits(dest, dest_trits);

  prng_absorbn(prng->sponge, 5, trits_to_absorb);
  prng_squeeze(prng->sponge, output);
}

void prng_destroy(prng_t *const prng) {
  prng->sponge = NULL;
  memset(prng->key, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
}
