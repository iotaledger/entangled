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

void mam_prng_init(mam_prng_t *const prng, trits_t const secret_key) {
  MAM2_ASSERT(trits_size(secret_key) == MAM2_PRNG_KEY_SIZE);

  trits_copy(secret_key, trits_from_rep(MAM2_PRNG_KEY_SIZE, prng->secret_key));
}

void mam_prng_gen(mam_prng_t const *const prng, tryte_t const destination,
                  trits_t const nonce, trits_t output) {
  mam_prng_gen3(prng, destination, nonce, trits_null(), trits_null(), output);
}

void mam_prng_gen2(mam_prng_t const *const prng, tryte_t const destination,
                   trits_t const nonce1, trits_t const nonce2, trits_t output) {
  mam_prng_gen3(prng, destination, nonce1, nonce2, trits_null(), output);
}

void mam_prng_gen3(mam_prng_t const *const prng, tryte_t const destination,
                   trits_t const nonce1, trits_t const nonce2,
                   trits_t const nonce3, trits_t output) {
  mam_sponge_t sponge;
  trits_t KdN[5];
  MAM2_TRITS_DEF0(dt, 3);
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = trits_from_rep(MAM2_PRNG_KEY_SIZE, prng->secret_key);
  KdN[1] = dt;
  KdN[2] = nonce1;
  KdN[3] = nonce2;
  KdN[4] = nonce3;
  trits_put3(dt, destination);

  mam_sponge_init(&sponge);
  mam_sponge_absorbn(&sponge, MAM2_SPONGE_CTL_KEY, 5, KdN);
  mam_sponge_squeeze(&sponge, MAM2_SPONGE_CTL_PRN, output);
}
