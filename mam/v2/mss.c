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

#include "mam/v2/mss.h"

/*
 * Private functions
 */

/*!< [in] hash values of left and right child nodes */
/*!< [out] hash value of their parent */
static void mss_mt_hash2(isponge *sponge, trits_t h[2], trits_t h01) {
  dbg_printf("\nleft  \t");
  trits_dbg_print(h[0]);
  dbg_printf("\nright \t");
  trits_dbg_print(h[1]);

#if defined(MAM2_MSS_DEBUG)
  {
    // this is a special debug MT hash function
    mss_mt_index_t i0, i1, i01;
    // hash of the node is it's index in level
    i0 = trits_get18(h[0]);
    // MAM2_ASSERT(i0 == ns[s->stack_size].index);
    i1 = trits_get18(h[1]);
    // MAM2_ASSERT(i1 == ns[s->s+1].index);
    // left node is even
    MAM2_ASSERT(i0 % 2 == 0);
    // right node is left node + 1
    MAM2_ASSERT(i0 + 1 == i1);
    // parent's index
    i01 = i0 / 2;
    trits_set_zero(h01);
    trits_put18(h01, i01);
  }
#else
  sponge_hashn(sponge, 2, h, h01);
#endif

  dbg_printf("\nparent\t");
  trits_dbg_print(h01);
  dbg_printf("\n");
}

/*!< [in] leaf index: `0 <= i < 2^D` */
/*!< [out] WOTS pk / leaf hash */
static void mss_mt_gen_leaf(mss_t *mss, mss_mt_index_t index, trits_t h) {
  MAM2_ASSERT(0 <= index && index <= MAM2_MSS_MAX_SKN(mss->height));

#if defined(MAM2_MSS_DEBUG)
  // this is a special debug MT hash function;
  // hash of the node is it's index in the level
  trits_set_zero(h);
  trits_put18(h, index);
#else
  MAM2_TRITS_DEF(Ni, 18);
  // gen sk from current leaf index
  trits_put18(Ni, index);
  // TODO Remove when mss handles flex_trits
  TRIT_ARRAY_MAKE_FROM_RAW(nonce1, mss->nonce1.n - mss->nonce1.d,
                           mss->nonce1.p + mss->nonce1.d);
  TRIT_ARRAY_MAKE_FROM_RAW(nonce2, mss->nonce2.n - mss->nonce2.d,
                           mss->nonce2.p + mss->nonce2.d);
  TRIT_ARRAY_MAKE_FROM_RAW(noncei, Ni.n - Ni.d, Ni.p + Ni.d);
  wots_gen_sk3(mss->wots, mss->prng, &nonce1, &nonce2, &noncei);
  // calc pk & push hash
  wots_calc_pk(mss->wots, h);
#endif

  dbg_printf("wpk %d   \t", index);
  trits_dbg_print(h);
  dbg_printf("\n");
}

#if defined(MAM2_MSS_TRAVERSAL)
static trits_t mss_hash_idx(trit_t *p, size_t i) {
  return trits_from_rep(MAM2_MSS_MT_HASH_SIZE, p + MAM2_MSS_HASH_IDX(i));
}

static trits_t mss_mt_ap_trits(mss_t *mss, mss_mt_height_t d) {
  return mss_hash_idx(mss->auth_path, d);
}

static trits_t mss_mt_hs_trits(mss_t *mss, mss_mt_height_t d, size_t i) {
  return mss_hash_idx(mss->hashes, MAM2_MSS_MT_NODES(d) + i);
}

static void mss_mt_init(mss_t *mss) {
  mss_mt_stack_t *s = mss->stacks;
  mss_mt_height_t d;

  for (d = 0; d < mss->height; d++, s++) {
    s->height = d;
    s->index = 0;
    s->stack_size = 0;
  }
}

static void mss_mt_update(mss_t *mss, mss_mt_height_t d) {
  mss_mt_stack_t *s;
  mss_mt_node_t *ns;
  trit_t *hs;
  trits_t h[2], wpk;

  // current level must be lower than MT height
  MAM2_ASSERT(0 <= d && d < mss->height);
  // stack at level `d`
  s = mss->stacks + MAM2_MSS_MT_STACKS(d);
  // stack's nodes
  ns = mss->nodes + MAM2_MSS_MT_NODES(d);
  // stack's hashes stored separately from nodes
  hs = mss->hashes + MAM2_MSS_MT_HASH_WORDS(d, 0);

  // finished?
  if ((0 != s->stack_size) && (ns[s->stack_size - 1].height >= s->height))
    return;

  // can merge (top 2 nodes have the same height)?
  if (s->stack_size > 1 && (ns[s->stack_size - 2].height ==
                            ns[s->stack_size - 1].height)) {  // merge
    // pop the right node
    h[1] = mss_hash_idx(hs, --s->stack_size);
    // pop the left node
    h[0] = mss_hash_idx(hs, --s->stack_size);

    // hash them
    // dirty hack: at the last level do not overwrite
    // left hash value, but instead right;
    // left hash value is needed for MTT algorithm
    dbg_printf("mt  d=%d i=%d\t", ns[s->stack_size].height,
               ns[s->stack_size].index);
    mss_mt_hash2(mss->sponge, h,
                 h[ns[s->stack_size].height + 1 != mss->height ? 0 : 1]);

    // push parent into stack
    // parent is one level up
    ns[s->stack_size].height += 1;
    // parent's index
    ns[s->stack_size].index /= 2;
    // adjust stack size
    s->stack_size++;
  } else if (s->index <= MAM2_MSS_MAX_SKN(mss->height)) {
    // pk will be put on top of the stack
    wpk = mss_hash_idx(hs, s->stack_size);
    mss_mt_gen_leaf(mss, s->index, wpk);

    // push leaf into stack
    // leaf has level `0`
    ns[s->stack_size].height = 0;
    // leaf's index
    ns[s->stack_size].index = s->index;
    // increase stack size
    s->stack_size++;
    // increment leaf index (skn)
    s->index++;
  }
}

static void mss_mt_refresh(mss_t *mss) {
  mss_mt_stack_t *s;
  mss_mt_height_t d;
  mss_mt_index_t dd;

  for (d = 0; d < mss->height; d++) {
    dd = (mss_mt_index_t)1 << d;
    if ((mss->skn + 1) % dd != 0) break;

    s = mss->stacks + d;
    MAM2_ASSERT(s->stack_size == 1);
    trits_copy(mss_mt_hs_trits(mss, d, s->stack_size - 1),
               mss_mt_ap_trits(mss, d));

    s->index = (mss->skn + 1 + dd) ^ dd;
    s->height = d;
    s->stack_size = 0;
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
    isponge *s,
    mss_mt_index_t skn, /*!< [in] corresponding WOTS sk number / leaf index */
    trits_t ap,         /*!< [in] authentication path - leaf to root */
    trits_t h           /*!< [in] recovered WOTS pk / start hash value;
                             [out] recovered MT root */
) {
  trits_t t[2];

  for (; !trits_is_empty(ap);
       skn /= 2, ap = trits_drop(ap, MAM2_MSS_MT_HASH_SIZE)) {
    t[skn % 2] = h;
    t[1 - (skn % 2)] = trits_take(ap, MAM2_MSS_MT_HASH_SIZE);
    dbg_printf("mt  i=%d \t", skn);
    mss_mt_hash2(s, t, h);
  }
}

/*
 * Public functions
 */

void mss_init(mss_t *const mss, prng_t *const prng, isponge *const sponge,
              wots_t *const wots, mss_mt_height_t const height,
              trits_t const nonce1, trits_t const nonce2) {
  MAM2_ASSERT(mss);
  MAM2_ASSERT(prng);
  MAM2_ASSERT(0 <= height && height <= MAM2_MSS_MAX_HEIGHT);

  mss->height = height;
  mss->skn = 0;
  mss->prng = prng;
  mss->sponge = sponge;
  mss->wots = wots;
  mss->nonce1 = nonce1;
  mss->nonce2 = nonce2;
#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_init(mss);
#endif
}

void mss_gen(mss_t *mss, trits_t pk) {
#if defined(MAM2_MSS_TRAVERSAL)
  // reuse stack `D-1`, by construction (see note in mss.h)
  // it has capacity `D+1`
  mss_mt_height_t d = mss->height - 1;
  mss_mt_stack_t *s = mss->stacks + MAM2_MSS_MT_STACKS(d);
  mss_mt_node_t *ns = mss->nodes + MAM2_MSS_MT_NODES(d), *n;
  trit_t *hs = mss->hashes + MAM2_MSS_MT_HASH_WORDS(d, 0);

  // init stack
  // max node height is `D`
  s->height = mss->height;
  // start leaf index (skn) is `0`
  s->index = 0;
  // empty stack
  s->stack_size = 0;

  if (0 == mss->height) {
    mss_mt_gen_leaf(mss, 0, pk);
  } else
    for (;;) {
      // update current stack
      mss_mt_update(mss, d);

      // top node
      n = ns + (s->stack_size - 1);
      // is it root?
      if (n->height == mss->height) {  // done
        // copy pk, it is stored outside of stack due to dirty hack
        trits_t h = mss_hash_idx(hs, s->stack_size);
        trits_copy(h, pk);
        // init stack
        s->height = mss->height - 1;
        s->index = 0;
        s->stack_size = 1;
        break;
      }

      // is it current apath node?
      if (n->index == 1) {  // add to `ap`
        trits_t h = mss_hash_idx(hs, s->stack_size - 1);
        trits_t a = mss_mt_ap_trits(mss, n->height);
        trits_copy(h, a);
      } else

          // is it next apath node?
          if (n->index == 0 &&
              (n->height + 1 != mss->height)) {  // push to stack `n->height`
        // stack `n->height`
        mss_mt_stack_t *sd = mss->stacks + MAM2_MSS_MT_STACKS(n->height);
        // stack `n->height` nodes
        mss_mt_node_t *nsd = mss->nodes + MAM2_MSS_MT_NODES(n->height);
        // node `n` hash
        trits_t h = mss_hash_idx(hs, s->stack_size - 1);
        // stack `n->height` first node hash
        trits_t hd = mss_mt_hs_trits(mss, n->height, 0);

        MAM2_ASSERT(0 == sd->stack_size);
        // copy hash
        trits_copy(h, hd);
        // copy node
        nsd[0] = *n;
        // increase size
        sd->stack_size++;
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

void mss_skn(mss_t *mss, trits_t skn) {
  MAM2_TRITS_DEF(ts, 18);

  MAM2_ASSERT(trits_size(skn) == MAM2_MSS_SKN_SIZE);

  trits_put6(ts, mss->height);
  trits_copy(trits_take(ts, 4), trits_take(skn, 4));

  trits_put18(ts, mss->skn);
  trits_copy(trits_take(ts, 14), trits_drop(skn, 4));

  dbg_printf("skn\n");
  trits_dbg_print(skn);
  dbg_printf("\n");
}

void mss_apath(mss_t *mss, trint18_t i, trits_t p) {
  mss_mt_height_t d;

  MAM2_ASSERT(trits_size(p) == MAM2_MSS_AUTH_PATH_SIZE(mss->height));

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
    trits_t pi = trits_take(p, MAM2_MSS_MT_HASH_SIZE);
#if defined(MAM2_MSS_TRAVERSAL)
    trits_t ni = mss_hash_idx(mss->auth_path, d);
#else
    trits_t ni = mss_mt_node_t_trits(m, d, (0 == i % 2) ? i + 1 : i - 1);
#endif
    trits_copy(ni, pi);

    dbg_printf("ap\t");
    trits_dbg_print(ni);
    dbg_printf("\n");

    p = trits_drop(p, MAM2_MSS_MT_HASH_SIZE);
  }
}

void mss_sign(mss_t *mss, trits_t hash, trits_t sig) {
  MAM2_ASSERT(trits_size(sig) == MAM2_MSS_SIG_SIZE(mss->height));

  dbg_printf("mss sign skn=%d\n", mss->skn);
  mss_skn(mss, trits_take(sig, MAM2_MSS_SKN_SIZE));
  sig = trits_drop(sig, MAM2_MSS_SKN_SIZE);

#if defined(MAM2_MSS_DEBUG)
  // instead of WOTS sig gen WOTS pk directly
  mss_mt_gen_leaf(mss, mss->skn, trits_take(sig, MAM2_MSS_MT_HASH_SIZE));
#else
  {
    MAM2_TRITS_DEF(Ni, 18);
    trits_put18(Ni, mss->skn);
    // TODO Remove when mss handles flex_trits
    TRIT_ARRAY_MAKE_FROM_RAW(nonce1, mss->nonce1.n - mss->nonce1.d,
                             mss->nonce1.p + mss->nonce1.d);
    TRIT_ARRAY_MAKE_FROM_RAW(nonce2, mss->nonce2.n - mss->nonce2.d,
                             mss->nonce2.p + mss->nonce2.d);
    TRIT_ARRAY_MAKE_FROM_RAW(noncei, Ni.n - Ni.d, Ni.p + Ni.d);
    wots_gen_sk3(mss->wots, mss->prng, &nonce1, &nonce2, &noncei);
  }

  flex_trits_to_trits(sig.p + sig.d, MAM2_WOTS_SK_SIZE, mss->wots->sk,
                      MAM2_WOTS_SK_SIZE, MAM2_WOTS_SK_SIZE);
  TRIT_ARRAY_MAKE_FROM_RAW(hash_array, MAM2_WOTS_HASH_SIZE, hash.p + hash.d);
  TRIT_ARRAY_MAKE_FROM_RAW(sk_sig_array, MAM2_WOTS_SIG_SIZE, sig.p + sig.d);
  wots_sign(mss->wots, &hash_array, &sk_sig_array);
  flex_trits_to_trits(sig.p + sig.d, MAM2_WOTS_SIG_SIZE, sk_sig_array.trits,
                      MAM2_WOTS_SIG_SIZE, MAM2_WOTS_SIG_SIZE);
#endif
  sig = trits_drop(sig, MAM2_WOTS_SIG_SIZE);

  mss_apath(mss, mss->skn, sig);
}

bool_t mss_next(mss_t *mss) {
  if (mss->skn == MAM2_MSS_MAX_SKN(mss->height)) return 0;

#if defined(MAM2_MSS_TRAVERSAL)
  if (mss->skn < MAM2_MSS_MAX_SKN(mss->height)) {
    mss_mt_refresh(mss);
    mss_mt_build_stacks(mss);
  }
#endif

  mss->skn++;
  return 1;
}

bool_t mss_verify(isponge *ms, isponge *ws, trits_t H, trits_t sig,
                  trits_t pk) {
  trint6_t d;
  trint18_t skn;
  MAM2_TRITS_DEF(ts, 18);
  MAM2_TRITS_DEF(apk, MAM2_MSS_MT_HASH_SIZE);

  dbg_printf("mss verify\n");

  if (trits_size(sig) < MAM2_MSS_SIG_SIZE(0)) return 0;

  trits_set_zero(ts);

  trits_copy(trits_take(sig, 4), trits_take(ts, 4));
  sig = trits_drop(sig, 4);
  d = trits_get6(ts);

  trits_copy(trits_take(sig, 14), trits_take(ts, 14));
  sig = trits_drop(sig, 14);
  skn = trits_get18(ts);

  dbg_printf("d=%d skn=%d", d, skn);
  if (d < 0 || skn < 0 || skn >= (1 << d) ||
      trits_size(sig) != (MAM2_MSS_SIG_SIZE(d) - MAM2_MSS_SKN_SIZE)) {
    return 0;
  }

#if defined(MAM2_MSS_DEBUG)
  trits_copy(trits_take(sig, MAM2_MSS_MT_HASH_SIZE), apk);
#else
  wots_recover(ws, H, trits_take(sig, MAM2_WOTS_SIG_SIZE), apk);
#endif
  dbg_printf("\nwpR\t");
  trits_dbg_print(apk);
  sig = trits_drop(sig, MAM2_WOTS_SIG_SIZE);

  mss_fold_apath(ms, skn, sig, apk);

  dbg_printf("apk\t");
  trits_dbg_print(apk);
  dbg_printf("\npk\t");
  trits_dbg_print(pk);
  dbg_printf("\n*************\n");

  return (0 == trits_cmp_grlex(apk, pk)) ? 1 : 0;
}

retcode_t mss_create(mss_t *mss, mss_mt_height_t d) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  MAM2_ASSERT(mss);

  do {
    memset(mss, 0, sizeof(mss_t));
    err_guard(0 <= d && d <= MAM2_MSS_MAX_HEIGHT, RC_MAM2_INVALID_ARGUMENT);

#if defined(MAM2_MSS_TRAVERSAL)
    mss->auth_path =
        (trit_t *)malloc(sizeof(trit_t) * MAM2_MSS_MT_AUTH_WORDS(d));
    err_guard(mss->auth_path, RC_OOM);

    // add 1 extra hash for dirty hack (see mss.c)
    mss->hashes =
        (trit_t *)malloc(sizeof(trit_t) * MAM2_MSS_MT_HASH_WORDS(d, 1));
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
