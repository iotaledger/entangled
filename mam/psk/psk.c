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
#include "mam/pb3/pb3.h"
#include "mam/psk/mam_psk_t_set.h"
#include "utils/memset_safe.h"

retcode_t mam_psk_init(mam_psk_t *const psk, mam_prng_t const *const prng,
                       tryte_t const *const id, tryte_t const *const nonce,
                       size_t const nonce_length) {
  MAM_TRITS_DEF0(nonce_trits, MAM_SPONGE_RATE);
  nonce_trits = MAM_TRITS_INIT(nonce_trits, MAM_SPONGE_RATE);

  if (psk == NULL || prng == NULL || id == NULL || nonce == NULL) {
    return RC_NULL_PARAM;
  }

  trits_from_str(mam_psk_id(psk), id);
  nonce_trits = trits_take_min(nonce_trits, nonce_length * 3);
  trits_from_str(nonce_trits, nonce);
  mam_prng_gen(prng, MAM_PRNG_DST_SEC_KEY, nonce_trits, mam_psk_key(psk));

  return RC_OK;
}

void mam_psk_destroy(mam_psk_t *const psk) {
  if (psk == NULL) {
    return;
  }

  memset_safe(psk->key, MAM_PSK_KEY_SIZE, 0, MAM_PSK_KEY_SIZE);
}

trits_t mam_psk_id(mam_psk_t const *const psk) {
  if (psk == NULL) {
    return trits_null();
  }

  return trits_from_rep(MAM_PSK_ID_SIZE, psk->id);
}

trits_t mam_psk_key(mam_psk_t const *const psk) {
  if (psk == NULL) {
    return trits_null();
  }

  return trits_from_rep(MAM_PSK_KEY_SIZE, psk->key);
}

void mam_psks_destroy(mam_psk_t_set_t *const psks) {
  mam_psk_t_set_entry_t *entry = NULL;
  mam_psk_t_set_entry_t *tmp = NULL;

  if (psks == NULL || *psks == NULL) {
    return;
  }

  SET_ITER(*psks, entry, tmp) {
    mam_psk_destroy(&entry->value);
    mam_psk_t_set_remove_entry(psks, entry);
  }
}

size_t mam_psks_serialized_size(mam_psk_t_set_t const psks) {
  if (psks == NULL) {
    return 0;
  }

  return pb3_sizeof_size_t(mam_psk_t_set_size(psks)) +
         mam_psk_t_set_size(psks) * (MAM_PSK_ID_SIZE + MAM_PSK_KEY_SIZE);
}

retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t *const trits) {
  mam_psk_t_set_entry_t *entry = NULL;
  mam_psk_t_set_entry_t *tmp = NULL;

  if (psks == NULL || trits == NULL) {
    return RC_NULL_PARAM;
  }

  pb3_encode_size_t(mam_psk_t_set_size(psks), trits);

  SET_ITER(psks, entry, tmp) {
    pb3_encode_ntrytes(trits_from_rep(MAM_PSK_ID_SIZE, entry->value.id), trits);
    pb3_encode_ntrytes(trits_from_rep(MAM_PSK_KEY_SIZE, entry->value.key),
                       trits);
  }

  return RC_OK;
}

retcode_t mam_psks_deserialize(trits_t *const trits,
                               mam_psk_t_set_t *const psks) {
  retcode_t ret = RC_OK;
  size_t psks_size = 0;
  mam_psk_t psk;

  if (trits == NULL || psks == NULL) {
    return RC_NULL_PARAM;
  }

  ERR_BIND_RETURN(pb3_decode_size_t(&psks_size, trits), ret);

  for (size_t i = 0; i < psks_size; i++) {
    ERR_BIND_RETURN(
        pb3_decode_ntrytes(trits_from_rep(MAM_PSK_ID_SIZE, psk.id), trits),
        ret);
    ERR_BIND_RETURN(
        pb3_decode_ntrytes(trits_from_rep(MAM_PSK_KEY_SIZE, psk.key), trits),
        ret);
    ERR_BIND_RETURN(mam_psk_t_set_add(psks, &psk), ret);
  }

  return ret;
}
