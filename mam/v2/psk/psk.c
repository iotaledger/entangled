/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/psk/psk.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/psk/mam_psk_t_set.h"

trits_t mam_psk_id(mam_psk_t const *const psk) {
  return trits_from_rep(MAM2_PSK_ID_SIZE, psk->id);
}

trits_t mam_psk_trits(mam_psk_t const *const psk) {
  return trits_from_rep(MAM2_PSK_KEY_SIZE, psk->key);
}

size_t mam_psks_serialized_size(mam_psk_t_set_t const psks) {
  return mam_psk_t_set_size(psks) * sizeof(mam_psk_t);
}

retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t trits) {
  mam_psk_t_set_entry_t *entry = NULL;
  mam_psk_t_set_entry_t *tmp = NULL;

  HASH_ITER(hh, psks, entry, tmp) {
    pb3_encode_ntrytes(trits_from_rep(MAM2_PSK_ID_SIZE, entry->value.id),
                       &trits);
    pb3_encode_ntrytes(trits_from_rep(MAM2_PSK_KEY_SIZE, entry->value.key),
                       &trits);
  }

  return RC_OK;
}

retcode_t mam_psks_deserialize(trits_t const trits,
                               mam_psk_t_set_t *const psks) {
  retcode_t ret = RC_OK;
  trits_t cpy = trits;
  mam_psk_t psk;

  while (!trits_is_empty(cpy)) {
    pb3_decode_ntrytes(trits_from_rep(MAM2_PSK_ID_SIZE, psk.id), &cpy);
    pb3_decode_ntrytes(trits_from_rep(MAM2_PSK_KEY_SIZE, psk.key), &cpy);
    if ((ret = mam_psk_t_set_add(psks, &psk)) != RC_OK) {
      break;
    }
  }

  return ret;
}
