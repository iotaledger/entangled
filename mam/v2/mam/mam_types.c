/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/mam/mam_types.h"
#include "mam/v2/mam/mam_pre_shared_key_t_set.h"

size_t psks_serialized_size(mam_pre_shared_key_t_set_t const psks) {
  return mam_pre_shared_key_t_set_size(psks) * sizeof(mam_pre_shared_key_t);
}

retcode_t psks_serialize(mam_pre_shared_key_t_set_t const psks, trits_t trits) {
  mam_pre_shared_key_t_set_entry_t *entry = NULL;
  mam_pre_shared_key_t_set_entry_t *tmp = NULL;

  HASH_ITER(hh, psks, entry, tmp) {
    trits_copy(trits_from_rep(MAM2_PSK_ID_SIZE, entry->value.id),
               trits_take(trits, MAM2_PSK_ID_SIZE));
    trits = trits_drop(trits, MAM2_PSK_ID_SIZE);
    trits_copy(trits_from_rep(MAM2_PSK_SIZE, entry->value.pre_shared_key),
               trits_take(trits, MAM2_PSK_SIZE));
    trits = trits_drop(trits, MAM2_PSK_SIZE);
  }

  return RC_OK;
}

retcode_t psks_deserialize(trits_t const trits,
                           mam_pre_shared_key_t_set_t *const psks) {
  retcode_t ret = RC_OK;
  trits_t cpy = trits;
  mam_pre_shared_key_t psk;

  while (!trits_is_empty(cpy)) {
    trits_copy(trits_take(cpy, MAM2_PSK_ID_SIZE),
               trits_from_rep(MAM2_PSK_ID_SIZE, psk.id));
    cpy = trits_drop(cpy, MAM2_PSK_ID_SIZE);
    trits_copy(trits_take(cpy, MAM2_PSK_SIZE),
               trits_from_rep(MAM2_PSK_SIZE, psk.pre_shared_key));
    cpy = trits_drop(cpy, MAM2_PSK_SIZE);
    if ((ret = mam_pre_shared_key_t_set_add(psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}
