/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/prng/prng_types.h"
#include "mam/v2/prng/mam_prng_t_set.h"

size_t mam_prng_serialized_size() { return MAM2_PRNG_KEY_SIZE; }

void mam_prng_serialize(mam_prng_t const *const prng, trits_t trits) {
  trits_copy(trits_from_rep(MAM2_PRNG_KEY_SIZE, prng->secret_key), trits);
}

void mam_prng_deserialize(trits_t const trits, mam_prng_t *const prng) {
  trits_copy(trits, trits_from_rep(MAM2_PRNG_KEY_SIZE, prng->secret_key));
}

size_t mam_prngs_serialized_size(mam_prng_t_set_t const prng_set) {
  return mam_prng_t_set_size(prng_set) * mam_prng_serialized_size();
}

retcode_t mam_prngs_serialize(mam_prng_t_set_t const prng_set, trits_t trits) {
  mam_prng_t_set_entry_t *entry = NULL;
  mam_prng_t_set_entry_t *tmp = NULL;

  HASH_ITER(hh, prng_set, entry, tmp) {
    mam_prng_serialize(&entry->value, trits_take(trits, MAM2_PRNG_KEY_SIZE));
    trits = trits_drop(trits, MAM2_PRNG_KEY_SIZE);
  }

  return RC_OK;
}

retcode_t mam_prngs_deserialize(trits_t const trits,
                                mam_prng_t_set_t *const prng_set) {
  retcode_t ret = RC_OK;
  trits_t cpy = trits;
  mam_prng_t prng;

  while (!trits_is_empty(cpy)) {
    mam_prng_deserialize(trits_take(cpy, mam_prng_serialized_size()), &prng);
    cpy = trits_drop(cpy, mam_prng_serialized_size());
    if ((ret = mam_prng_t_set_add(prng_set, &prng)) != RC_OK) {
      break;
    }
  }

  return ret;
}