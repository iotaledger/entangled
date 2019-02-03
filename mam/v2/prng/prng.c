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
#include <string.h>

#include "mam/v2/prng/mam_prng_t_set.h"
#include "mam/v2/prng/prng.h"

/*
 * Private functions
 */

static inline void prng_absorbn(sponge_t *const sponge, size_t const n,
                                trits_t const *const KdN) {
  sponge_init(sponge);
  sponge_absorbn(sponge, MAM2_SPONGE_CTL_KEY, n, KdN);
}

static inline void prng_squeeze(sponge_t *const sponge, trits_t output) {
  sponge_squeeze(sponge, MAM2_SPONGE_CTL_PRN, output);
}

static inline trits_t prng_secret_key_trits(mam_prng_t const *const prng) {
  return trits_from_rep(MAM2_PRNG_KEY_SIZE, prng->secret_key);
}

/*
 * Public functions
 */

retcode_t prng_create(mam_prng_t *const prng) {
  retcode_t ret = RC_OK;
  MAM2_ASSERT(prng);

  prng->sponge = NULL;
  if ((prng->secret_key = calloc(MAM2_PRNG_KEY_SIZE, sizeof(trit_t))) == NULL) {
    return RC_OOM;
  }

  return ret;
}

void prng_destroy(mam_prng_t *const prng) {
  MAM2_ASSERT(prng);

  prng->sponge = NULL;
  free(prng->secret_key);
  prng->secret_key = NULL;
}

void prng_init(mam_prng_t *const prng, sponge_t *const sponge,
               trits_t const secret_key) {
  MAM2_ASSERT(trits_size(secret_key) == MAM2_PRNG_KEY_SIZE);

  prng->sponge = sponge;
  trits_copy(secret_key, prng_secret_key_trits(prng));
}

void prng_gen(mam_prng_t const *const prng, tryte_t const destination,
              trits_t const nonce, trits_t output) {
  prng_gen3(prng, destination, nonce, trits_null(), trits_null(), output);
}

void prng_gen2(mam_prng_t const *const prng, tryte_t const destination,
               trits_t const nonce1, trits_t const nonce2, trits_t output) {
  prng_gen3(prng, destination, nonce1, nonce2, trits_null(), output);
}

void prng_gen3(mam_prng_t const *const prng, tryte_t const destination,
               trits_t const nonce1, trits_t const nonce2, trits_t const nonce3,
               trits_t output) {
  trits_t KdN[5];
  MAM2_TRITS_DEF0(dt, 3);
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = prng_secret_key_trits(prng);
  KdN[1] = dt;
  KdN[2] = nonce1;
  KdN[3] = nonce2;
  KdN[4] = nonce3;
  trits_put3(dt, destination);

  prng_absorbn(prng->sponge, 5, KdN);
  prng_squeeze(prng->sponge, output);
}
