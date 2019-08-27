/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "mam/mss/mss.h"
#include "mam/pb3/pb3.h"

/*!< [in] hash values of left and right child nodes */
/*!< [out] hash value of their parent */
void mss_hash2(mam_spongos_t *s, trits_t children_hashes[2], trits_t parent_hash) {
  mam_spongos_hashn(s, 2, children_hashes, parent_hash);
}

/*!< [in] leaf index: `0 <= i < 2^D` */
/*!< [out] WOTS pk / leaf hash */
void mss_mt_gen_leaf(mam_mss_t *mss, mss_mt_idx_t i, trits_t pk) {
  mam_wots_t wots;
  mam_sponge_t sponge;

  MAM_TRITS_DEF(nonce_i, MAM_MSS_SKN_SIZE);
  mam_sponge_init(&sponge);
  mam_wots_reset(&wots);

  MAM_ASSERT(0 <= i && i <= MAM_MSS_MAX_SKN(mss->height));
  nonce_i = MAM_TRITS_INIT(nonce_i, MAM_MSS_SKN_SIZE);

  /* gen sk from current leaf index */
  trits_put18(nonce_i, i);
  mam_wots_gen_sk5(&wots, mss->prng, mss->nonce1, mss->nonce2, mss->nonce3, mss->nonce4, nonce_i);
  /* gen pk & push hash */
  mam_wots_gen_pk(&wots, pk);
}

retcode_t mam_mss_sign(mam_mss_t *mss, trits_t hash, trits_t sig) {
  mam_wots_t wots;
  mam_sponge_t sponge;
  MAM_ASSERT(trits_size(sig) == MAM_MSS_SIG_SIZE(mss->height));

  if (mam_mss_remaining_sks(mss) == 0) {
    return RC_MAM_MSS_EXHAUSTED;
  }

  // Write both tree height and the sk index to the signature
  mam_mss_skn(mss, trits_take(sig, MAM_MSS_SKN_SIZE));
  sig = trits_drop(sig, MAM_MSS_SKN_SIZE);

  mam_sponge_init(&sponge);
  mam_wots_reset(&wots);
  {
    // Generate the current (skn) secret key
    MAM_TRITS_DEF(nonce_i, MAM_MSS_SKN_SIZE);
    nonce_i = MAM_TRITS_INIT(nonce_i, MAM_MSS_SKN_SIZE);
    trits_put18(nonce_i, mss->skn);
    mam_wots_gen_sk5(&wots, mss->prng, mss->nonce1, mss->nonce2, mss->nonce3, mss->nonce4, nonce_i);
  }

  mam_wots_sign(&wots, hash, trits_take(sig, MAM_WOTS_SIGNATURE_SIZE));
  sig = trits_drop(sig, MAM_WOTS_SIGNATURE_SIZE);

  mam_mss_auth_path(mss, mss->skn, sig);

  return RC_OK;
}

retcode_t mam_mss_sign_and_next(mam_mss_t *mss, trits_t hash, trits_t sig) {
  retcode_t ret;
  ERR_BIND_RETURN(mam_mss_sign(mss, hash, sig), ret);
  mam_mss_next(mss);
  return RC_OK;
}

void mam_mss_skn(mam_mss_t const *const mss, trits_t skn) {
  MAM_TRITS_DEF(trits, MAM_MSS_SKN_SIZE);
  trits = MAM_TRITS_INIT(trits, MAM_MSS_SKN_SIZE);

  MAM_ASSERT(trits_size(skn) == MAM_MSS_SKN_SIZE);

  trits_put6(trits, mss->height);
  trits_copy(trits_take(trits, MAM_MSS_SKN_TREE_DEPTH_SIZE), trits_take(skn, MAM_MSS_SKN_TREE_DEPTH_SIZE));

  trits_put18(trits, mss->skn);
  trits_copy(trits_take(trits, MAM_MSS_SKN_KEY_NUMBER_SIZE), trits_drop(skn, MAM_MSS_SKN_TREE_DEPTH_SIZE));
}

bool mam_mss_verify(mam_spongos_t *mt_spongos, trits_t hash, trits_t sig, trits_t pk) {
  mss_mt_height_t height;
  mss_mt_idx_t skn;
  MAM_TRITS_DEF(calculated_pk, MAM_MSS_MT_HASH_SIZE);
  calculated_pk = MAM_TRITS_INIT(calculated_pk, MAM_MSS_MT_HASH_SIZE);

  MAM_ASSERT(trits_size(pk) == MAM_MSS_PK_SIZE);

  if (trits_size(sig) < MAM_MSS_SIG_SIZE(0)) return false;

  if (!mss_parse_skn(&height, &skn, trits_take(sig, MAM_MSS_SKN_SIZE))) {
    return false;
  }

  sig = trits_drop(sig, MAM_MSS_SKN_SIZE);
  if (trits_size(sig) != (size_t)(MAM_MSS_SIG_SIZE(height) - MAM_MSS_SKN_SIZE)) return false;

  mam_wots_recover(hash, trits_take(sig, MAM_WOTS_SIGNATURE_SIZE), calculated_pk);
  sig = trits_drop(sig, MAM_WOTS_SIGNATURE_SIZE);

  mss_fold_auth_path(mt_spongos, skn, sig, calculated_pk);

  return trits_cmp_eq(calculated_pk, pk);
}

bool mss_parse_skn(mss_mt_height_t *height, mss_mt_idx_t *skn, trits_t trits) {
  MAM_TRITS_DEF(ts, MAM_MSS_SKN_SIZE);
  ts = MAM_TRITS_INIT(ts, MAM_MSS_SKN_SIZE);

  MAM_ASSERT(MAM_MSS_SKN_SIZE == trits_size(trits));

  trits_set_zero(ts);

  trits_copy(trits_take(trits, MAM_MSS_SKN_TREE_DEPTH_SIZE), trits_take(ts, MAM_MSS_SKN_TREE_DEPTH_SIZE));
  *height = trits_get6(ts);

  trits_copy(trits_drop(trits, MAM_MSS_SKN_TREE_DEPTH_SIZE), trits_take(ts, MAM_MSS_SKN_KEY_NUMBER_SIZE));
  *skn = trits_get18(ts);

  if (*height > MAM_MSS_MAX_D || *skn > MAM_MSS_MAX_SKN(*height)) return 0;

  return 1;
}

/**
 * Calculate the mss root (pk)
 *
 * @param spongos [in] The spongos that is used to hash nodes on the tree
 * @param skn [in] number of WOTS instance (current pk index), in traversal mode
 * @param auth_path [in] authentication path - leaf to root
 * @param pk [in] recovered WOTS pk / start hash value [out] recovered MT root
 *
 * @return void
 */

void mss_fold_auth_path(mam_spongos_t *spongos, mss_mt_idx_t skn, trits_t auth_path, trits_t pk) {
  trits_t hashes[2];

  for (; !trits_is_empty(auth_path); skn /= 2, auth_path = trits_drop(auth_path, MAM_MSS_MT_HASH_SIZE)) {
    hashes[skn % 2] = pk;
    hashes[1 - (skn % 2)] = trits_take(auth_path, MAM_MSS_MT_HASH_SIZE);
    mss_hash2(spongos, hashes, pk);
  }
}

size_t mam_mss_remaining_sks(mam_mss_t const *const mss) {
  if (mss->skn > MAM_MSS_MAX_SKN(mss->height)) {
    return 0;
  }

  return MAM_MSS_MAX_SKN(mss->height) - mss->skn + 1;
}

size_t mam_mss_serialized_size(mam_mss_t const *const mss) {
  return MAM_MSS_SKN_TREE_DEPTH_SIZE + MAM_MSS_SKN_KEY_NUMBER_SIZE + mss_mt_serialized_size(mss) + MAM_MSS_PK_SIZE;
}

void mam_mss_serialize(mam_mss_t const *const mss, trits_t *const buffer) {
  MAM_ASSERT(trits_size(*buffer) >= mam_mss_serialized_size(mss));

  mam_mss_skn(mss, trits_take(*buffer, MAM_MSS_SKN_SIZE));
  trits_advance(buffer, MAM_MSS_SKN_SIZE);
  mss_mt_serialize(mss, buffer);
  pb3_encode_ntrytes(trits_from_rep(MAM_MSS_PK_SIZE, mss->root), buffer);
}
