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

static trits_t mss_mt_node_t_trits(mam_mss_t const *const mss, mss_mt_height_t height, mss_mt_idx_t i) {
  MAM_ASSERT(height <= mss->height);
  MAM_ASSERT(i < ((mss_mt_idx_t)1 << height));

  size_t idx = ((size_t)1 << height) + i - 1;
  trit_t *t = mss->mt + MAM_MSS_MT_HASH_SIZE * idx;
  return trits_from_rep(MAM_MSS_MT_HASH_SIZE, t);
}

void mam_mss_init(mam_mss_t *mss, mam_prng_t *const prng, mss_mt_height_t height, trits_t nonce1, trits_t nonce2,
                  trits_t nonce3, trits_t nonce4) {
  MAM_ASSERT(mss);
  MAM_ASSERT(prng);
  MAM_ASSERT(0 <= height && height <= MAM_MSS_MAX_D);

  mss->height = height;
  mss->skn = 0;
  mss->prng = prng;
  mss->nonce1 = nonce1;
  mss->nonce2 = nonce2;
  mss->nonce3 = nonce3;
  mss->nonce4 = nonce4;
}

void mam_mss_gen(mam_mss_t *mss) {
  trits_t root_trits = trits_from_rep(MAM_MSS_PK_SIZE, mss->root);
  mss_mt_height_t height;
  mss_mt_idx_t i, n;
  mam_spongos_t spongos;

  for (i = 0, n = (mss_mt_idx_t)1 << mss->height; i < n; ++i) {
    trits_t wpk = mss_mt_node_t_trits(mss, mss->height, i);
    mss_mt_gen_leaf(mss, i, wpk);
  }

  for (height = mss->height; height--;) {
    for (i = 0, n = (mss_mt_idx_t)1 << height; i < n; ++i) {
      trits_t h[2], h01;
      h[0] = mss_mt_node_t_trits(mss, height + 1, 2 * i + 0);
      h[1] = mss_mt_node_t_trits(mss, height + 1, 2 * i + 1);
      h01 = mss_mt_node_t_trits(mss, height, i);

      mss_hash2(&spongos, h, h01);
    }
  }

  trits_copy(mss_mt_node_t_trits(mss, 0, 0), root_trits);
}

void mam_mss_auth_path(mam_mss_t *mss, mss_mt_idx_t skn, trits_t path) {
  mss_mt_height_t height;
  trits_t path_part_out;

  MAM_ASSERT(trits_size(path) == MAM_MSS_APATH_SIZE(mss->height));

  /* note, level height is reversed (compared to traversal): */
  /* `0` is root level, `hieght` is leaf level */
  for (height = mss->height; height; --height, skn = skn / 2) {
    path_part_out = trits_take(path, MAM_MSS_MT_HASH_SIZE);
    trits_t curr_auth_path_part = mss_mt_node_t_trits(mss, height, (0 == skn % 2) ? skn + 1 : skn - 1);
    trits_copy(curr_auth_path_part, path_part_out);
    path = trits_drop(path, MAM_MSS_MT_HASH_SIZE);
  }
}

bool mam_mss_next(mam_mss_t *mss) {
  if (mam_mss_remaining_sks(mss) == 0) {
    return false;
  }

  mss->skn++;

  return true;
}

retcode_t mam_mss_create(mam_mss_t *mss, mss_mt_height_t height) {
  MAM_ASSERT(mss);

  memset(mss, 0, sizeof(mam_mss_t));
  ERR_GUARD_RETURN(0 <= height && height <= MAM_MSS_MAX_D, RC_MAM_INVALID_ARGUMENT);

  mss->mt = malloc(sizeof(trit_t) * MAM_MSS_MT_WORDS(height));
  ERR_GUARD_RETURN(mss->mt, RC_OOM);

  /* do not free here in case of error */
  return RC_OK;
}

void mam_mss_destroy(mam_mss_t *mss) {
  MAM_ASSERT(mss);

  if (mss->mt) {
    free(mss->mt);
    mss->mt = NULL;
  }
}

size_t mss_mt_serialized_size(mam_mss_t const *const mss) {
  return (((size_t)1 << (mss->height + 1)) - 1) * MAM_MSS_MT_HASH_SIZE;
}

void mss_mt_serialize(mam_mss_t const *const mss, trits_t *buffer) {
  mss_mt_height_t height;
  mss_mt_idx_t i;

  MAM_ASSERT(trits_size(*buffer) >= mss_mt_serialized_size(mss));

  height = mss->height;
  do {
    for (i = 0; i != (mss_mt_idx_t)1 << height; ++i) {
      trits_copy(mss_mt_node_t_trits(mss, height, i), trits_take(*buffer, MAM_MSS_MT_HASH_SIZE));
      *buffer = trits_drop(*buffer, MAM_MSS_MT_HASH_SIZE);
    }
  } while (height-- > 0);
}

static void mss_mt_deserialize(trits_t buffer, mam_mss_t *mss) {
  mss_mt_height_t height;
  mss_mt_idx_t i;

  MAM_ASSERT(trits_size(buffer) == mss_mt_serialized_size(mss));

  /* <node-hashes> */
  height = mss->height;
  do {
    for (i = 0; i != (mss_mt_idx_t)1 << height; ++i) {
      trits_copy(trits_take(buffer, MAM_MSS_MT_HASH_SIZE), mss_mt_node_t_trits(mss, height, i));
      buffer = trits_drop(buffer, MAM_MSS_MT_HASH_SIZE);
    }
  } while (height-- > 0);
}

retcode_t mam_mss_deserialize(trits_t *const buffer, mam_mss_t *const mss) {
  retcode_t ret = RC_OK;
  mss_mt_height_t height;
  mss_mt_idx_t skn;

  ERR_GUARD_RETURN(MAM_MSS_SKN_TREE_DEPTH_SIZE + MAM_MSS_SKN_KEY_NUMBER_SIZE <= trits_size(*buffer),
                   RC_MAM_BUFFER_TOO_SMALL);
  ERR_GUARD_RETURN(
      mss_parse_skn(&height, &skn, trits_advance(buffer, MAM_MSS_SKN_TREE_DEPTH_SIZE + MAM_MSS_SKN_KEY_NUMBER_SIZE)),
      RC_MAM_INVALID_VALUE);
  ERR_GUARD_RETURN(height == mss->height, RC_MAM_INVALID_VALUE);

  mss->skn = skn;
  ERR_GUARD_RETURN(mss_mt_serialized_size(mss) <= trits_size(*buffer), RC_MAM_BUFFER_TOO_SMALL);
  mss_mt_deserialize(trits_advance(buffer, mss_mt_serialized_size(mss)), mss);

  if ((ret = pb3_decode_ntrytes(trits_from_rep(MAM_MSS_PK_SIZE, mss->root), buffer)) != RC_OK) {
    return ret;
  }

  return ret;
}
