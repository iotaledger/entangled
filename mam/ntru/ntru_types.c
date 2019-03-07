/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/ntru/mam_ntru_pk_t_set.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/ntru/ntru.h"
#include "mam/pb3/pb3.h"

size_t mam_ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pk_set) {
  return pb3_sizeof_size_t(mam_ntru_pk_t_set_size(ntru_pk_set)) +
         mam_ntru_pk_t_set_size(ntru_pk_set) * sizeof(mam_ntru_pk_t);
}

retcode_t mam_ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pk_set,
                                 trits_t *const trits) {
  mam_ntru_pk_t_set_entry_t *entry = NULL;
  mam_ntru_pk_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_ntru_pk_t_set_size(ntru_pk_set), trits);
  HASH_ITER(hh, ntru_pk_set, entry, tmp) {
    pb3_encode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, entry->value.key),
                       trits);
  }

  return RC_OK;
}

retcode_t mam_ntru_pks_deserialize(trits_t *const trits,
                                   mam_ntru_pk_t_set_t *const ntru_pk_set) {
  retcode_t ret = RC_OK;
  mam_ntru_pk_t ntru_pk;

  size_t container_size = 0;
  pb3_decode_size_t(&container_size, trits);

  while (!trits_is_empty(*trits) && container_size-- > 0) {
    pb3_decode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, ntru_pk.key), trits);
    if ((ret = mam_ntru_pk_t_set_add(ntru_pk_set, &ntru_pk)) != RC_OK) {
      break;
    }
  }

  return ret;
}

void mam_ntru_sks_destroy(mam_ntru_sk_t_set_t *const ntru_sks) {
  mam_ntru_sk_t_set_entry_t *entry = NULL;
  mam_ntru_sk_t_set_entry_t *tmp = NULL;

  if (ntru_sks == NULL || *ntru_sks == NULL) {
    return;
  }

  HASH_ITER(hh, *ntru_sks, entry, tmp) { ntru_destroy(&entry->value); }
  mam_ntru_sk_t_set_free(ntru_sks);
}

size_t mam_ntru_sks_serialized_size(mam_ntru_sk_t_set_t const ntru_sk_set) {
  return pb3_sizeof_size_t(mam_ntru_sk_t_set_size(ntru_sk_set)) +
         mam_ntru_sk_t_set_size(ntru_sk_set) *
             (MAM_NTRU_PK_SIZE + MAM_NTRU_SK_SIZE);
}

retcode_t mam_ntru_sks_serialize(mam_ntru_sk_t_set_t const ntru_sk_set,
                                 trits_t *const trits) {
  mam_ntru_sk_t_set_entry_t *entry = NULL;
  mam_ntru_sk_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_ntru_sk_t_set_size(ntru_sk_set), trits);

  HASH_ITER(hh, ntru_sk_set, entry, tmp) {
    pb3_encode_ntrytes(
        trits_from_rep(MAM_NTRU_PK_SIZE, entry->value.public_key.key), trits);
    trits_copy(trits_from_rep(MAM_NTRU_SK_SIZE, entry->value.secret_key),
               trits_take(*trits, MAM_NTRU_SK_SIZE));
    trits_advance(trits, MAM_NTRU_SK_SIZE);
  }

  return RC_OK;
}

retcode_t mam_ntru_sks_deserialize(trits_t *const trits,
                                   mam_ntru_sk_t_set_t *const ntru_sk_set) {
  retcode_t ret = RC_OK;
  mam_ntru_sk_t ntru_sk;
  size_t container_size = 0;

  ntru_init(&ntru_sk);
  pb3_decode_size_t(&container_size, trits);

  while (!trits_is_empty(*trits) && container_size-- > 0) {
    pb3_decode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, ntru_sk.public_key.key),
                       trits);
    trits_copy(trits_take(*trits, MAM_NTRU_SK_SIZE),
               trits_from_rep(MAM_NTRU_SK_SIZE, ntru_sk.secret_key));
    *trits = trits_drop(*trits, MAM_NTRU_SK_SIZE);
    ntru_load_sk(&ntru_sk);
    if ((ret = mam_ntru_sk_t_set_add(ntru_sk_set, &ntru_sk)) != RC_OK) {
      break;
    }
  }

  ntru_destroy(&ntru_sk);

  return ret;
}
