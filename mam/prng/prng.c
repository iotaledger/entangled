/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/prng/prng.h"
#include "mam/sponge/sponge.h"
#include "utils/memset_safe.h"

retcode_t mam_prng_init(mam_prng_t *const prng, trits_t const secret_key) {
  if (prng == NULL) {
    return RC_NULL_PARAM;
  }

  if (trits_size(secret_key) != MAM_PRNG_KEY_SIZE) {
    return RC_INVALID_PARAM;
  }

  memcpy(prng->secret_key, trits_begin(secret_key), MAM_PRNG_KEY_SIZE);

  return RC_OK;
}

retcode_t mam_prng_reset(mam_prng_t *const prng) {
  if (prng == NULL) {
    return RC_NULL_PARAM;
  }

  memset_safe(prng->secret_key, MAM_PRNG_KEY_SIZE, 0, MAM_PRNG_KEY_SIZE);

  return RC_OK;
}

retcode_t mam_prng_gen3(mam_prng_t const *const prng, mam_prng_destination_tryte_t const destination,
                        trits_t const nonce1, trits_t const nonce2, trits_t const nonce3, trits_t output) {
  mam_sponge_t sponge;
  trits_t KdN[5];
  trit_t dt[3];

  if (prng == NULL) {
    return RC_NULL_PARAM;
  }

  KdN[0] = trits_from_rep(MAM_PRNG_KEY_SIZE, prng->secret_key);
  KdN[1] = trits_from_rep(3, dt);
  KdN[2] = nonce1;
  KdN[3] = nonce2;
  KdN[4] = nonce3;
  trits_put3(KdN[1], destination);

  mam_sponge_init(&sponge);
  mam_sponge_absorbn(&sponge, MAM_SPONGE_CTL_KEY, 5, KdN);
  mam_sponge_squeeze(&sponge, MAM_SPONGE_CTL_PRN, output);

  return RC_OK;
}
