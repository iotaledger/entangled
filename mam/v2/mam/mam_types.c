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
#include "mam/v2/mam/mam_ntru_pk_t_set.h"
#include "mam/v2/mam/mam_ntru_sk_t_set.h"
#include "mam/v2/mam/mam_pre_shared_key_t_set.h"

size_t mam_psks_serialized_size(mam_pre_shared_key_t_set_t const psks) {
  return mam_pre_shared_key_t_set_size(psks) * sizeof(mam_pre_shared_key_t);
}

retcode_t mam_psks_serialize(mam_pre_shared_key_t_set_t const psks,
                             trits_t trits) {
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

retcode_t mam_psks_deserialize(trits_t const trits,
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
      break;
    }
  }

  return ret;
}

size_t mam_ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pk_set) {
  return mam_ntru_pk_t_set_size(ntru_pk_set) * sizeof(mam_ntru_pk_t);
}

retcode_t mam_ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pk_set,
                                 trits_t trits) {
  mam_ntru_pk_t_set_entry_t *entry = NULL;
  mam_ntru_pk_t_set_entry_t *tmp = NULL;

  HASH_ITER(hh, ntru_pk_set, entry, tmp) {
    trits_copy(trits_from_rep(MAM2_NTRU_PK_SIZE, entry->value.pk),
               trits_take(trits, MAM2_NTRU_PK_SIZE));
    trits = trits_drop(trits, MAM2_NTRU_PK_SIZE);
  }

  return RC_OK;
}

retcode_t mam_ntru_pks_deserialize(trits_t const trits,
                                   mam_ntru_pk_t_set_t *const ntru_pk_set) {
  retcode_t ret = RC_OK;
  trits_t cpy = trits;
  mam_ntru_pk_t ntru_pk;

  while (!trits_is_empty(cpy)) {
    trits_copy(trits_take(cpy, MAM2_NTRU_PK_SIZE),
               trits_from_rep(MAM2_NTRU_PK_SIZE, ntru_pk.pk));
    cpy = trits_drop(cpy, MAM2_NTRU_PK_SIZE);
    if ((ret = mam_ntru_pk_t_set_add(ntru_pk_set, &ntru_pk)) != RC_OK) {
      break;
    }
  }

  return ret;
}

size_t mam_ntru_sk_serialized_size(mam_ntru_sk_t_set_t const ntru_sk_set) {
  return mam_ntru_sk_t_set_size(ntru_sk_set) *
         (MAM2_NTRU_ID_SIZE + MAM2_NTRU_SK_SIZE);
}

retcode_t mam_ntru_sk_serialize(mam_ntru_sk_t_set_t const ntru_sk_set,
                                trits_t trits) {
  mam_ntru_sk_t_set_entry_t *entry = NULL;
  mam_ntru_sk_t_set_entry_t *tmp = NULL;

  HASH_ITER(hh, ntru_sk_set, entry, tmp) {
    trits_copy(trits_from_rep(MAM2_NTRU_ID_SIZE, entry->value.public_key_id),
               trits_take(trits, MAM2_NTRU_ID_SIZE));
    trits = trits_drop(trits, MAM2_NTRU_ID_SIZE);
    trits_copy(trits_from_rep(MAM2_NTRU_SK_SIZE, entry->value.secret_key),
               trits_take(trits, MAM2_NTRU_SK_SIZE));
    trits = trits_drop(trits, MAM2_NTRU_SK_SIZE);
  }

  return RC_OK;
}

retcode_t mam_ntru_sk_deserialize(trits_t const trits,
                                  mam_ntru_sk_t_set_t *const ntru_sk_set) {
  retcode_t ret = RC_OK;
  trits_t cpy = trits;
  mam_ntru_sk_t ntru_sk;

  while (!trits_is_empty(cpy)) {
    trits_copy(trits_take(cpy, MAM2_NTRU_ID_SIZE),
               trits_from_rep(MAM2_NTRU_ID_SIZE, ntru_sk.public_key_id));
    cpy = trits_drop(cpy, MAM2_NTRU_ID_SIZE);
    trits_copy(trits_take(cpy, MAM2_NTRU_SK_SIZE),
               trits_from_rep(MAM2_NTRU_SK_SIZE, ntru_sk.secret_key));
    cpy = trits_drop(cpy, MAM2_NTRU_SK_SIZE);
    if ((ret = mam_ntru_sk_t_set_add(ntru_sk_set, &ntru_sk)) != RC_OK) {
      break;
    }
  }

  return ret;
}
