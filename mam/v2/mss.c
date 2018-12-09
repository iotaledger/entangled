/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/trinary/trit_long.h"
#include "mam/v2/mss.h"

/*
 * Private functions
 */

/*!< [in] hash values of left and right child nodes */
/*!< [out] hash value of their parent */
static void mss_mt_hash2(sponge_t *sponge, trit_array_t hashes_pack[2],
                         trit_array_p hash) {
  sponge_hashn(sponge, 2, hashes_pack, hash);
}

/*!< [in] leaf index: `0 <= i < 2^D` */
/*!< [out] WOTS pk / leaf hash */
static void mss_mt_gen_leaf(mss_t *mss, mss_mt_index_t index, trit_array_p pk) {
  MAM2_ASSERT(0 <= index && index <= MAM2_MSS_MAX_SKN(mss->height));
  // gen sk from current leaf index
  trit_t nonce_i[MAM2_MSS_SKN_SIZE] = {0};
  long_to_trits(index, nonce_i);
  TRIT_ARRAY_MAKE_FROM_RAW(noncei, MAM2_MSS_SKN_SIZE, nonce_i);
  wots_gen_sk3(mss->wots, mss->prng, &mss->nonce1, &mss->nonce2, &noncei);
  // calc pk & push hash
  wots_calc_pk(mss->wots, pk);

  dbg_printf("wpk %d   \t", index);
  trits_dbg_print(pk);
  dbg_printf("\n");
}

#if defined(MAM2_MSS_TRAVERSAL)

static void mss_mt_set_auth_node(mss_t *mss, mss_mt_height_t d,
                                 trit_array_t *const auth_node) {
  flex_trits_insert_from_pos(mss->auth_path,
                             MAM2_MSS_AUTH_PATH_SIZE(mss->height),
                             auth_node->trits, MAM2_MSS_MT_HASH_SIZE, 0,
                             d * MAM2_MSS_MT_HASH_SIZE, MAM2_MSS_MT_HASH_SIZE);
}

static void mss_mt_get_hash(mss_t *const mss, mss_mt_height_t const height,
                            mss_mt_index_t const index,
                            trit_array_t *const hash) {
  flex_trits_insert_from_pos(
      hash->trits, MAM2_MSS_MT_HASH_SIZE, mss->hashes,
      MAM2_MSS_MT_HASH_WORDS(mss->height, 1),
      (MAM2_MSS_MT_NODES(height) + index) * MAM2_MSS_MT_HASH_SIZE, 0,
      MAM2_MSS_MT_HASH_SIZE);
}

static void mss_mt_set_hash(mss_t *const mss, mss_mt_height_t const height,
                            mss_mt_index_t const index,
                            trit_array_t const *const hash) {
  flex_trits_insert_from_pos(
      mss->hashes, MAM2_MSS_MT_HASH_WORDS(mss->height, 1), hash->trits,
      MAM2_MSS_MT_HASH_SIZE, 0,
      (MAM2_MSS_MT_NODES(height) + index) * MAM2_MSS_MT_HASH_SIZE,
      MAM2_MSS_MT_HASH_SIZE);
}

static void mss_mt_init(mss_t *mss) {
  mss_mt_stack_t *s = mss->stacks;
  mss_mt_height_t d;

  for (d = 0; d < mss->height; d++, s++) {
    s->height = d;
    s->index = 0;
    s->size = 0;
  }
}

static void mss_mt_update(mss_t *mss, mss_mt_height_t d) {
  // current level must be lower than MT height
  MAM2_ASSERT(0 <= d && d < mss->height);
  // stack at level d
  mss_mt_stack_t *stack = &mss->stacks[MAM2_MSS_MT_STACKS(d)];
  // stack's nodes
  mss_mt_node_t *nodes = &mss->nodes[MAM2_MSS_MT_NODES(d)];

  // finished?
  if ((0 != stack->size) && (nodes[stack->size - 1].height >= stack->height))
    return;

  // can merge (top 2 nodes have the same height)?
  if (stack->size > 1 && (nodes[stack->size - 2].height ==
                          nodes[stack->size - 1].height)) {  // merge
    trit_array_t hashes[2];

    // pop the left node
    stack->size--;
    TRIT_ARRAY_DECLARE(hash0, MAM2_MSS_MT_HASH_SIZE);
    mss_mt_get_hash(mss, d, stack->size, &hash0);
    hashes[0] = hash0;

    // pop the right node
    stack->size--;
    TRIT_ARRAY_DECLARE(hash1, MAM2_MSS_MT_HASH_SIZE);
    mss_mt_get_hash(mss, d, stack->size, &hash1);
    hashes[1] = hash1;

    // hash them
    // dirty hack: at the last level do not overwrite
    // left hash value, but instead right;
    // left hash value is needed for MTT algorithm
    dbg_printf("mt  d=%d i=%d\t", nodes[stack->size].height,
               nodes[stack->size].index);

    TRIT_ARRAY_DECLARE(hash, MAM2_MSS_MT_HASH_SIZE);
    mss_mt_hash2(mss->sponge, hashes, &hash);
    mss_mt_set_hash(
        mss, d,
        stack->size + (nodes[stack->size].height + 1 != mss->height ? 0 : 1),
        &hash);

    // push parent into stack
    // parent is one level up
    nodes[stack->size].height += 1;
    // parent's index
    nodes[stack->size].index /= 2;
    // adjust stack size
    stack->size++;
  } else if (stack->index <= MAM2_MSS_MAX_SKN(mss->height)) {
    // pk will be put on top of the stack
    TRIT_ARRAY_DECLARE(wpk, MAM2_WOTS_PK_SIZE);
    mss_mt_gen_leaf(mss, stack->index, &wpk);
    mss_mt_set_hash(mss, d, stack->size, &wpk);

    // push leaf into stack
    // leaf has level `0`
    nodes[stack->size].height = 0;
    // leaf's index
    nodes[stack->size].index = stack->index;
    // increase stack size
    stack->size++;
    // increment leaf index (skn)
    stack->index++;
  }
}

static void mss_mt_refresh(mss_t *mss) {
  mss_mt_stack_t *stack;
  mss_mt_height_t d;
  mss_mt_index_t dd;
  TRIT_ARRAY_DECLARE(hash, MAM2_MSS_MT_HASH_SIZE);

  for (d = 0; d < mss->height; d++) {
    dd = (mss_mt_index_t)1 << d;
    if ((mss->skn + 1) % dd != 0) break;

    stack = &mss->stacks[d];
    MAM2_ASSERT(stack->size == 1);
    mss_mt_get_hash(mss, d, stack->size - 1, &hash);
    mss_mt_set_auth_node(mss, d, &hash);

    stack->index = (mss->skn + 1 + dd) ^ dd;
    stack->height = d;
    stack->size = 0;
  }
}

static void mss_mt_build_stacks(mss_t *mss) {
  // classic Merkle tree traversal variant
  mss_mt_height_t d;
  for (d = 0; d < mss->height; ++d)
    mss_mt_update(mss, d), mss_mt_update(mss, d);
}
#else
static trits_t mss_mt_node_t_trits(mss_t *m, trint6_t d, trint18_t i) {
  MAM2_ASSERT(d <= mss->height);
  MAM2_ASSERT(i < ((trint18_t)1 << d));

  size_t idx = ((size_t)1 << d) + i - 1;
  trit_t *w = mss->merkle_tree + MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE) * idx;
  return trits_from_rep(MAM2_MSS_MT_HASH_SIZE, w);
}
#endif

static void mss_fold_apath(
    sponge_t *s,
    mss_mt_index_t skn, /*!< [in] corresponding WOTS sk number / leaf index */
    trit_array_p ap,    /*!< [in] authentication path - leaf to root */
    trit_array_p pk_recovered /*!< [in] recovered WOTS pk / start hash value;
                   [out] recovered MT root */
) {
  TRIT_ARRAY_DECLARE(h_el1, MAM2_MSS_MT_HASH_SIZE);
  TRIT_ARRAY_DECLARE(h_el2, MAM2_MSS_MT_HASH_SIZE);
  trit_array_t hashes_pack[2] = {h_el1, h_el2};

  size_t ap_length = ap->num_trits / MAM2_MSS_MT_HASH_SIZE;
  size_t pos = 0;
  for (size_t i = 0; i < ap_length; skn /= 2, ++i) {
    flex_trits_slice(hashes_pack[skn % 2].trits, MAM2_MSS_MT_HASH_SIZE,
                     pk_recovered->trits, MAM2_MSS_MT_HASH_SIZE, 0,
                     MAM2_MSS_MT_HASH_SIZE);

    flex_trits_slice(hashes_pack[1 - (skn % 2)].trits, MAM2_MSS_MT_HASH_SIZE,
                     ap->trits, ap_length * MAM2_MSS_MT_HASH_SIZE, pos,
                     MAM2_MSS_MT_HASH_SIZE);

    dbg_printf("mt  i=%d \t", skn);

    mss_mt_hash2(s, hashes_pack, pk_recovered);
    pos += MAM2_MSS_MT_HASH_SIZE;
  }
}

/*
 * Public functions
 */

retcode_t mss_create(mss_t *mss, mss_mt_height_t d) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  MAM2_ASSERT(mss);

  do {
    memset(mss, 0, sizeof(mss_t));
    err_guard(0 <= d && d <= MAM2_MSS_MAX_HEIGHT, RC_MAM2_INVALID_ARGUMENT);

#if defined(MAM2_MSS_TRAVERSAL)
    mss->auth_path = (flex_trit_t *)malloc(sizeof(flex_trit_t) *
                                           MAM2_MSS_AUTH_PATH_FLEX_SIZE(d));
    err_guard(mss->auth_path, RC_OOM);

    // add 1 extra hash for dirty hack (see mss.c)
    mss->hashes = (flex_trit_t *)malloc(
        sizeof(flex_trit_t) *
        NUM_FLEX_TRITS_FOR_TRITS(MAM2_MSS_MT_HASH_WORDS(d, 1)));
    err_guard(mss->hashes, RC_OOM);

    // add 1 extra node for dirty hack (see mss.c)
    mss->nodes = malloc(sizeof(mss_mt_node_t) * (MAM2_MSS_MT_NODES(d) + 1));
    err_guard(mss->nodes, RC_OOM);

    mss->stacks = malloc(sizeof(mss_mt_stack_t) * MAM2_MSS_MT_STACKS(d));
    err_guard(mss->nodes, RC_OOM);
#else
    mss->merkle_tree = (trit_t *)malloc(sizeof(trit_t) * MAM2_MSS_MT_WORDS(d));
    err_guard(mss->merkle_tree, RC_OOM);
#endif

    e = RC_OK;
  } while (0);

  // do not free here in case of error
  return e;
}

void mss_init(mss_t *const mss, prng_t *const prng, sponge_t *const sponge,
              wots_t *const wots, mss_mt_height_t const height,
              trit_array_p const nonce1, trit_array_p const nonce2) {
  MAM2_ASSERT(mss);
  MAM2_ASSERT(prng);
  MAM2_ASSERT(0 <= height && height <= MAM2_MSS_MAX_HEIGHT);

  mss->height = height;
  mss->skn = 0;
  mss->prng = prng;
  mss->sponge = sponge;
  mss->wots = wots;
  mss->nonce1.dynamic = 0;
  mss->nonce2.dynamic = 0;
  trit_array_set_trits(&mss->nonce1, nonce1->trits, nonce1->num_trits);
  trit_array_set_trits(&mss->nonce2, nonce2->trits, nonce2->num_trits);
#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_init(mss);
#endif
}

void mss_gen(mss_t *mss, trit_array_p pk) {
#if defined(MAM2_MSS_TRAVERSAL)
  // reuse stack `D-1`, by construction (see note in mss.h)
  // it has capacity `D+1`
  mss_mt_height_t d = mss->height - 1;
  mss_mt_stack_t *stack = &mss->stacks[MAM2_MSS_MT_STACKS(d)];
  mss_mt_node_t *nodes = &mss->nodes[MAM2_MSS_MT_NODES(d)];
  mss_mt_node_t *node = NULL;

  // init stack
  // max node height is `D`
  stack->height = mss->height;
  // start leaf index (skn) is `0`
  stack->index = 0;
  // empty stack
  stack->size = 0;

  if (0 == mss->height) {
    mss_mt_gen_leaf(mss, 0, pk);
  } else
    for (;;) {
      // update current stack
      mss_mt_update(mss, d);

      // top node
      node = &nodes[stack->size - 1];
      // is it root?
      if (node->height == mss->height) {  // done
        // copy pk, it is stored outside of stack due to dirty hack
        mss_mt_get_hash(mss, d, stack->size, pk);
        // init stack
        stack->height = mss->height - 1;
        stack->index = 0;
        stack->size = 1;
        break;
      }

      // is it current apath node?
      if (node->index == 1) {  // add to `ap`
        TRIT_ARRAY_DECLARE(hash, MAM2_MSS_MT_HASH_SIZE);
        mss_mt_get_hash(mss, d, stack->size - 1, &hash);
        mss_mt_set_auth_node(mss, node->height, &hash);
      }
      // is it next apath node?
      else if (node->index == 0 && (node->height + 1 != mss->height)) {
        // push to stack `n->height`
        // stack `n->height`
        mss_mt_stack_t *sd = mss->stacks + MAM2_MSS_MT_STACKS(node->height);
        // stack `n->height` nodes
        mss_mt_node_t *nsd = mss->nodes + MAM2_MSS_MT_NODES(node->height);
        // node `n` hash
        TRIT_ARRAY_DECLARE(hash, MAM2_MSS_MT_HASH_SIZE);
        mss_mt_get_hash(mss, d, stack->size - 1, &hash);
        mss_mt_set_hash(mss, node->height, 0, &hash);

        MAM2_ASSERT(0 == sd->size);
        // copy node
        nsd[0] = *node;
        // increase size
        sd->size++;
      }
    }

#else
  mss_mt_height_t d;
  mss_mt_index_t i, n;

  // TODO: check spec: <i>6 or <i>18?
  for (i = 0, n = (trint18_t)1 << mss->height; i < n; ++i) {
    trits_t wpk = mss_mt_node_t_trits(m, mss->height, i);
    mss_mt_gen_leaf(m, i, wpk);
  }

  for (d = mss->height; d--;) {
    for (i = 0, n = (trint18_t)1 << d; i < n; ++i) {
      trits_t h[2], h01;
      h[0] = mss_mt_node_t_trits(mss, d + 1, 2 * i + 0);
      h[1] = mss_mt_node_t_trits(mss, d + 1, 2 * i + 1);
      h01 = mss_mt_node_t_trits(mss, d, i);

      dbg_printf("mt  d=%d i=%d\t", d + 1, 2 * i + 0);
      mss_mt_hash2(mss->s, h, h01);
    }
  }

  trits_copy(mss_mt_node_t_trits(mss, 0, 0), pk);
#endif
}

void mss_skn(mss_t *mss, trit_array_p skn) {
  trit_t ts[MAM2_MSS_SKN_SIZE] = {0};
  long_to_trits(mss->height, ts);
  long_to_trits(mss->skn, &ts[4]);
  flex_trits_from_trits(skn->trits, MAM2_MSS_SKN_SIZE, ts, MAM2_MSS_SKN_SIZE,
                        MAM2_MSS_SKN_SIZE);
  dbg_printf("skn\n");
  trits_dbg_print(skn);
  dbg_printf("\n");
}

void mss_auth_path(mss_t *mss, trint18_t i, trit_array_t *const auth_path) {
  mss_mt_height_t d;
  size_t offset = 0;

  MAM2_ASSERT(auth_path->num_trits == MAM2_MSS_AUTH_PATH_SIZE(mss->height));

  dbg_printf("apath i=%d\n", i);

#if defined(MAM2_MSS_TRAVERSAL)
  // current apath is already stored in `mss->auth_path`
  for (d = 0; d < mss->height; ++d)
#else
  // note, level height is reversed (compared to traversal):
  // `0` is root level, `D` is leaf level
  for (d = mss->height; d; --d, i = i / 2)
#endif
  {
#if defined(MAM2_MSS_TRAVERSAL)
    flex_trits_insert_from_pos(
        auth_path->trits, MAM2_MSS_AUTH_PATH_SIZE(mss->height), mss->auth_path,
        MAM2_MSS_AUTH_PATH_SIZE(mss->height), offset, offset,
        MAM2_MSS_MT_HASH_SIZE);
#else
    trits_t ni = mss_mt_node_t_trits(m, d, (0 == i % 2) ? i + 1 : i - 1);
    flex_trits_from_trits(auth_path->trits + offset, MAM2_MSS_MT_HASH_SIZE,
                          ni.p + ni.d, MAM2_MSS_MT_HASH_SIZE,
                          MAM2_MSS_MT_HASH_SIZE);
#endif

    dbg_printf("ap\t");
    trits_dbg_print(ni);
    dbg_printf("\n");

    offset += MAM2_MSS_MT_HASH_SIZE;
  }
}

void mss_sign(mss_t *mss, trit_array_p hash, trit_array_p sig) {
  MAM2_ASSERT(sig->num_trits == MAM2_MSS_SIG_SIZE(mss->height));

  dbg_printf("mss sign skn=%d\n", mss->skn);
  mss_skn(mss, sig);
  {
    trit_t nonce_i[MAM2_MSS_SKN_SIZE] = {0};
    long_to_trits(mss->skn, nonce_i);
    TRIT_ARRAY_MAKE_FROM_RAW(noncei, MAM2_MSS_SKN_SIZE, nonce_i);
    wots_gen_sk3(mss->wots, mss->prng, &mss->nonce1, &mss->nonce2, &noncei);
  }

  TRIT_ARRAY_DECLARE(sk_sig_array, MAM2_WOTS_SK_SIZE);
  memcpy(sk_sig_array.trits, mss->wots->sk, MAM2_WOTS_SK_FLEX_SIZE);

  wots_sign(mss->wots, hash, &sk_sig_array);

  trit_array_insert_from_pos(sig, &sk_sig_array, 0, MAM2_MSS_SKN_SIZE,
                             sk_sig_array.num_trits);
  // TODO Remove when sig is a trit_array_t
  TRIT_ARRAY_DECLARE(auth_path,
                     sig->num_trits - MAM2_MSS_SKN_SIZE - MAM2_WOTS_SIG_SIZE);
  mss_auth_path(mss, mss->skn, &auth_path);
  trit_array_insert_from_pos(sig, &auth_path, 0,
                             MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE,
                             auth_path.num_trits);
}

bool mss_next(mss_t *mss) {
  if (mss->skn == MAM2_MSS_MAX_SKN(mss->height)) {
    return false;
  };

#if defined(MAM2_MSS_TRAVERSAL)
  if (mss->skn < MAM2_MSS_MAX_SKN(mss->height)) {
    mss_mt_refresh(mss);
    mss_mt_build_stacks(mss);
  }
#endif

  mss->skn++;
  return true;
}

bool mss_verify(sponge_t *ms, sponge_t *ws, trit_array_p hash, trit_array_p sig,
                trit_array_p pk) {
  uint16_t d;
  uint32_t skn;
  trit_t tmp[MAM2_MSS_SKN_SIZE];

  TRIT_ARRAY_DECLARE(sk_sig_prefix_d, 4);
  TRIT_ARRAY_DECLARE(sk_sig_prefix_skn, 14);

  trit_array_insert_from_pos(&sk_sig_prefix_d, sig, 0, 0, 4);
  trit_array_insert_from_pos(&sk_sig_prefix_skn, sig, 4, 0, 14);
  flex_trits_to_trits(tmp, MAM2_MSS_SKN_SIZE, sk_sig_prefix_d.trits, 4, 4);
  d = trits_to_long(tmp, 4);
  flex_trits_to_trits(tmp, MAM2_MSS_SKN_SIZE, sk_sig_prefix_skn.trits, 14, 14);
  skn = trits_to_long(tmp, 14);

  size_t ap_size = sig->num_trits - MAM2_WOTS_SIG_SIZE - MAM2_MSS_SKN_SIZE;
  TRIT_ARRAY_DECLARE(sk_sig_array, MAM2_WOTS_SIG_SIZE);
  trit_array_insert_from_pos(&sk_sig_array, sig, MAM2_MSS_SKN_SIZE, 0,
                             MAM2_WOTS_SIG_SIZE);
  TRIT_ARRAY_DECLARE(ap_array, ap_size);
  trit_array_insert_from_pos(
      &ap_array, sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE, 0, ap_size);

  dbg_printf("mss verify\n");

  if (sig->num_trits < MAM2_MSS_SIG_SIZE(0)) {
    return false;
  };

  dbg_printf("d=%d skn=%d", d, skn);
  if (d < 0 || skn < 0 || skn >= (1 << d) ||
      (sk_sig_array.num_trits + ap_array.num_trits) !=
          (MAM2_MSS_SIG_SIZE(d) - MAM2_MSS_SKN_SIZE)) {
    return false;
  }

  MAM2_ASSERT(pk->num_trits == MAM2_WOTS_PK_SIZE);

  TRIT_ARRAY_DECLARE(apk, MAM2_MSS_MT_HASH_SIZE);
  wots_recover(ws, hash, &sk_sig_array, &apk);

  dbg_printf("\nwpR\t");

  mss_fold_apath(ms, skn, &ap_array, &apk);

  dbg_printf("apk\t");
  trits_dbg_print(apk);
  dbg_printf("\npk\t");
  trits_dbg_print(pk);
  dbg_printf("\n*************\n");

  return (0 == memcmp(pk->trits, apk.trits, MAM2_WOTS_PK_FLEX_SIZE));
}

void mss_destroy(mss_t *mss) {
  MAM2_ASSERT(mss);

#if defined(MAM2_MSS_TRAVERSAL)
  if (mss->auth_path) {
    free(mss->auth_path);
    mss->auth_path = NULL;
  }
  if (mss->hashes) {
    free(mss->hashes);
    mss->hashes = NULL;
  }
  if (mss->nodes) {
    free(mss->nodes);
    mss->nodes = NULL;
  }
  if (mss->stacks) {
    free(mss->stacks);
    mss->stacks = NULL;
  }
#else
  if (mss->merkle_tree) {
    free(mss->merkle_tree);
    mss->merkle_tree = NULL;
  }
#endif
}
