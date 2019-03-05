/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/psk/psk.h"
#include "mam/ntru/ntru.h"
#include "mam/pb3/pb3.h"
#include "mam/psk/mam_psk_t_set.h"

trits_t mam_psk_id(mam_psk_t const *const psk) {
  return trits_from_rep(MAM_PSK_ID_SIZE, psk->id);
}

trits_t mam_psk_trits(mam_psk_t const *const psk) {
  return trits_from_rep(MAM_PSK_KEY_SIZE, psk->key);
}

size_t mam_psks_serialized_size(mam_psk_t_set_t const psks) {
  return pb3_sizeof_size_t(mam_psk_t_set_size(psks)) +
         mam_psk_t_set_size(psks) * sizeof(mam_psk_t);
}

retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t *const trits) {
  mam_psk_t_set_entry_t *entry = NULL;
  mam_psk_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_psk_t_set_size(psks), trits);

  HASH_ITER(hh, psks, entry, tmp) {
    pb3_encode_ntrytes(trits_from_rep(MAM_PSK_ID_SIZE, entry->value.id), trits);
    pb3_encode_ntrytes(trits_from_rep(MAM_PSK_KEY_SIZE, entry->value.key),
                       trits);
  }

  return RC_OK;
}

retcode_t mam_psks_deserialize(trits_t *const trits,
                               mam_psk_t_set_t *const psks) {
  retcode_t ret = RC_OK;
  mam_psk_t psk;

  size_t container_size = 0;
  pb3_decode_size_t(&container_size, trits);

  while (!trits_is_empty(*trits) && container_size-- > 0) {
    pb3_decode_ntrytes(trits_from_rep(MAM_PSK_ID_SIZE, psk.id), trits);
    pb3_decode_ntrytes(trits_from_rep(MAM_PSK_KEY_SIZE, psk.key), trits);
    if ((ret = mam_psk_t_set_add(psks, &psk)) != RC_OK) {
      break;
    }
  }

  return ret;
}
