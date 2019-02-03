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
#include "mam/v2/ntru/mam_ntru_pk_t_set.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/pb3/pb3.h"

size_t mam_ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pk_set) {
  return mam_ntru_pk_t_set_size(ntru_pk_set) * sizeof(mam_ntru_pk_t);
}

retcode_t mam_ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pk_set,
                                 trits_t trits) {
  mam_ntru_pk_t_set_entry_t *entry = NULL;
  mam_ntru_pk_t_set_entry_t *tmp = NULL;

  HASH_ITER(hh, ntru_pk_set, entry, tmp) {
    pb3_encode_ntrytes(trits_from_rep(MAM2_NTRU_PK_SIZE, entry->value.pk),
                       &trits);
  }

  return RC_OK;
}

retcode_t mam_ntru_pks_deserialize(trits_t const trits,
                                   mam_ntru_pk_t_set_t *const ntru_pk_set) {
  retcode_t ret = RC_OK;
  mam_ntru_pk_t ntru_pk;

  while (!trits_is_empty(trits)) {
    pb3_decode_ntrytes(trits_from_rep(MAM2_NTRU_PK_SIZE, ntru_pk.pk), &trits);
    if ((ret = mam_ntru_pk_t_set_add(ntru_pk_set, &ntru_pk)) != RC_OK) {
      break;
    }
  }

  return ret;
}

size_t mam_ntru_sks_serialized_size(mam_ntru_sk_t_set_t const ntru_sk_set) {
  return mam_ntru_sk_t_set_size(ntru_sk_set) *
         (MAM2_NTRU_ID_SIZE + MAM2_NTRU_SK_SIZE);
}

retcode_t mam_ntru_sks_serialize(mam_ntru_sk_t_set_t const ntru_sk_set,
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

retcode_t mam_ntru_sks_deserialize(trits_t const trits,
                                   mam_ntru_sk_t_set_t *const ntru_sk_set) {
  retcode_t ret = RC_OK;
  trits_t cpy = trits;
  mam_ntru_sk_t ntru_sk;
  ntru_create(&ntru_sk);

  while (!trits_is_empty(cpy)) {
    trits_copy(trits_take(cpy, MAM2_NTRU_ID_SIZE),
               trits_from_rep(MAM2_NTRU_ID_SIZE, ntru_sk.public_key_id));
    cpy = trits_drop(cpy, MAM2_NTRU_ID_SIZE);
    trits_copy(trits_take(cpy, MAM2_NTRU_SK_SIZE),
               trits_from_rep(MAM2_NTRU_SK_SIZE, ntru_sk.secret_key));
    cpy = trits_drop(cpy, MAM2_NTRU_SK_SIZE);
    ntru_load_sk(&ntru_sk);
    if ((ret = mam_ntru_sk_t_set_add(ntru_sk_set, &ntru_sk)) != RC_OK) {
      break;
    }
  }

  ntru_destroy(&ntru_sk);

  return ret;
}
