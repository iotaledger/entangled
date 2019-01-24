/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <memory.h>
#include <stdlib.h>

#include "mam/v2/mss/mss.h"

#define MAM2_MSS_MAX_SKN(d) (((trint18_t)1 << (d)) - 1)

static void mss_hash2(
    spongos_t *s,
    trits_t hashes[2],    /*!< [in] hash values of left and right child nodes */
    trits_t hashed_hashes /*!< [out] hash value of their parent */
) {
#if defined(MAM2_MSS_DEBUG)
  {
    /* this is a special debug MT hash function */
    mss_mt_idx_t i0, i1, i01;
    /* hash of the node is it'spongos index in level */
    i0 = trits_get18(h[0]);
    /*MAM2_ASSERT(i0 == spongos_ntru[spongos->spongos].i); */
    i1 = trits_get18(h[1]);
    /*MAM2_ASSERT(i1 == spongos_ntru[spongos->spongos+1].i); */
    /* left node is even */
    MAM2_ASSERT(i0 % 2 == 0);
    /* right node is left node + 1 */
    MAM2_ASSERT(i0 + 1 == i1);
    /* parent'spongos index */
    i01 = i0 / 2;
    trits_set_zero(hashed_hashes);
    trits_put18(hashed_hashes, i01);
  }
#else
  spongos_hashn(s, 2, hashes, hashed_hashes);
#endif
}

static void mss_mt_hash2(mss_t *m, trits_t hashes[2], trits_t hashed_hashes) {
#if defined(MAM2_MSS_DEBUG)
  m->hash_node_count++;
#endif
  mss_hash2(m->sg, hashes, hashed_hashes);
}

static void mss_mt_gen_leaf(
    mss_t *mss, mss_mt_idx_t i, /*!< [in] leaf index: `0 <= i < 2^D` */
    trits_t pk                  /*!< [out] WOTS pk / leaf hash */
) {
#if defined(MAM2_MSS_DEBUG)
  MAM2_ASSERT(0 <= i && i <= MAM2_MSS_MAX_SKN(mss->height));

  mss->gen_leaf_count++;

  /* this is a special debug MT hash function; */
  /* hash of the node is it'spongos index in the level */
  trits_set_zero(h);
  trits_put18(h, i);
#else
  MAM2_TRITS_DEF0(nonce_i, MAM2_MSS_SKN_SIZE);

  MAM2_ASSERT(0 <= i && i <= MAM2_MSS_MAX_SKN(mss->height));
  nonce_i = MAM2_TRITS_INIT(nonce_i, MAM2_MSS_SKN_SIZE);

  /* gen sk from current leaf index */
  trits_put18(nonce_i, i);
  wots_gen_sk3(mss->wots, mss->prng, mss->nonce1, mss->nonce2, nonce_i);
  /* calc pk & push hash */
  wots_calc_pk(mss->wots, pk);
#endif
}

#if defined(MAM2_MSS_TRAVERSAL)
static trits_t mss_hash_idx(trit_t *p, size_t i) {
  return trits_from_rep(MAM2_MSS_MT_HASH_SIZE, p + MAM2_MSS_HASH_IDX(i));
}

static trits_t mss_mt_auth_path_trits(mss_t *mss, mss_mt_height_t height) {
  return mss_hash_idx(mss->auth_path, height);
}

static trits_t mss_mt_node_hash_trits(mss_t *mss, mss_mt_height_t height,
                                      size_t i) {
  return mss_hash_idx(mss->nodes_hashes, MAM2_MSS_MT_NODES(height) + i);
}

static void mss_mt_init(mss_t *mss) {
  mss_mt_stack_t *stack = mss->stacks;
  mss_mt_height_t height;

  for (height = 0; height < mss->height; height++, stack++) {
    stack->height = height;
    stack->level_idx = 0;
    stack->size = 0;
  }
}
/**
 * Peeks the stack of the mss at the current height and either
 * \note the nodes in `mss->nodes_hashes` are always pushed in order to
 * the stack, so, if the last two nodes in the stack are at the same height
 * then they must be the children of the same parent node, thus it make sense
 * to hash them together,
 *
 * @param mss [out] MSS interface
 * @param height [in] the current level
 *
 * @return void
 */

static void mss_mt_update(mss_t *mss, mss_mt_height_t height) {
  mss_mt_stack_t *stack;
  mss_mt_node_t *nodes;
  trit_t *nodes_hashes;
  trits_t hashes[2], wots_pk;

  /* current level must be lower than MT height */
  MAM2_ASSERT(0 <= height && height < mss->height);
  /* stack at level `height` */
  stack = mss->stacks + MAM2_MSS_MT_STACKS(height);
  /* stack'spongos nodes */
  nodes = mss->nodes + MAM2_MSS_MT_NODES(height);
  /* stack'spongos hashes stored separately from nodes */
  nodes_hashes = mss->nodes_hashes + MAM2_MSS_MT_HASH_WORDS(height, 0);

  /* finished? */
  if ((0 != stack->size) && (nodes[stack->size - 1].height >= stack->height)) {
    return;
  }

  /* can merge (top 2 nodes have the same height)? */
  if (stack->size > 1 && (nodes[stack->size - 2].height ==
                          nodes[stack->size - 1].height)) { /* merge */
    /* pop the right node */
    hashes[1] = mss_hash_idx(nodes_hashes, --stack->size);
    /* pop the left node */
    hashes[0] = mss_hash_idx(nodes_hashes, --stack->size);

    /* hash them */
    /* dirty hack: at the last level do not overwrite  */
    /* left hash value, but instead right; */
    /* left hash value is needed for MTT algorithm */
    mss_mt_hash2(mss, hashes,
                 hashes[nodes[stack->size].height + 1 != mss->height ? 0 : 1]);

    /* push parent into stack */
    /* parent is one level up */
    nodes[stack->size].height += 1;
    /* parent'spongos index */
    nodes[stack->size].level_idx /= 2;
    /* adjust stack size */
    stack->size++;
  } else if (stack->level_idx <= MAM2_MSS_MAX_SKN(mss->height)) {
    /* pk will be put on top of the stack */
    wots_pk = mss_hash_idx(nodes_hashes, stack->size);
    mss_mt_gen_leaf(mss, stack->level_idx, wots_pk);

    /* push leaf into stack */
    /* leaf has level `0` */
    nodes[stack->size].height = 0;
    /* leaf'spongos index */
    nodes[stack->size].level_idx = stack->level_idx;
    /* increase stack size */
    stack->size++;
    /* increment leaf index (skn) */
    stack->level_idx++;
  }
}

static void mss_mt_load_update(mss_t *mss, mss_mt_height_t height) {
  mss_mt_stack_t *stack;
  mss_mt_node_t *nodes;

  /* current level must be lower than MT height */
  MAM2_ASSERT(0 <= height && height < mss->height);
  /* stack at level `height` */
  stack = mss->stacks + MAM2_MSS_MT_STACKS(height);
  /* stack'spongos nodes */
  nodes = mss->nodes + MAM2_MSS_MT_NODES(height);

  /* finished? */
  if ((0 != stack->size) && (nodes[stack->size - 1].height >= stack->height))
    return;

  /* can merge (top 2 nodes have the same height)? */
  if (stack->size > 1 && (nodes[stack->size - 2].height ==
                          nodes[stack->size - 1].height)) { /* merge */
    /* pop the right node */
    --stack->size;
    /* pop the left node */
    --stack->size;

    /* hash them */

    /* push parent into stack */
    /* parent is one level up */
    nodes[stack->size].height += 1;
    /* parent'spongos index */
    nodes[stack->size].level_idx /= 2;
    /* adjust stack size */
    stack->size++;
  } else if (stack->level_idx <= MAM2_MSS_MAX_SKN(mss->height)) {
    /* pk will be put on top of the stack */

    /* push leaf into stack */
    /* leaf has level `0` */
    nodes[stack->size].height = 0;
    /* leaf'spongos index */
    nodes[stack->size].level_idx = stack->level_idx;
    /* increase stack size */
    stack->size++;
    /* increment leaf index (skn) */
    stack->level_idx++;
  }
}

static void mss_mt_refresh(mss_t *mss) {
  mss_mt_stack_t *stack;
  mss_mt_height_t height;
  mss_mt_idx_t dd;

  for (height = 0; height < mss->height; height++) {
    dd = (mss_mt_idx_t)1 << height;
    if ((mss->skn + 1) % dd != 0) {
      break;
    }

    stack = mss->stacks + height;
    MAM2_ASSERT(stack->size == 1);
    trits_copy(mss_mt_node_hash_trits(mss, height, stack->size - 1),
               mss_mt_auth_path_trits(mss, height));

    stack->level_idx = (mss->skn + 1 + dd) ^ dd;
    stack->height = height;
    stack->size = 0;
  }
}

static void mss_mt_load_refresh(mss_t *m) {
  mss_mt_stack_t *s;
  mss_mt_height_t d;
  mss_mt_idx_t dd;

  for (d = 0; d < m->height; d++) {
    dd = (mss_mt_idx_t)1 << d;
    if ((m->skn + 1) % dd != 0) break;

    s = m->stacks + d;
    MAM2_ASSERT(s->size == 1);

    s->level_idx = (m->skn + 1 + dd) ^ dd;
    s->height = d;
    s->size = 0;
  }
}

static void mss_mt_build_stacks(mss_t *mss) {
  /* classic Merkle tree traversal variant */
  mss_mt_height_t height;
  for (height = 0; height < mss->height; ++height) {
    mss_mt_update(mss, height), mss_mt_update(mss, height);
  }
}

static void mss_mt_load_build_stacks(mss_t *m) {
  mss_mt_height_t height;
  for (height = 0; height < m->height; ++height) {
    mss_mt_load_update(m, height), mss_mt_load_update(m, height);
  }
}

void mss_mt_rewind(mss_t *mss, trint18_t skn) {
  mss_mt_stack_t *stack;
  mss_mt_node_t *nodes;
  mss_mt_height_t height;

  for (height = 0; height < mss->height; ++height) {
    stack = mss->stacks + MAM2_MSS_MT_STACKS(height);
    nodes = mss->nodes + MAM2_MSS_MT_NODES(height);

    stack->height = height;
    stack->level_idx = 0;
    stack->size = 1;
    nodes->height = height;
    nodes->level_idx = 0;
  }

  if (mss->height > 0) {
    nodes->height++;
  }

  for (mss->skn = 0; mss->skn < skn; mss->skn++) {
    mss_mt_load_refresh(mss);
    mss_mt_load_build_stacks(mss);
  }
}
#else
static trits_t mss_mt_node_t_trits(mss_t *mss, trint6_t height, trint18_t i) {
  MAM2_ASSERT(height <= mss->height);
  MAM2_ASSERT(i < ((trint18_t)1 << height));

  size_t idx = ((size_t)1 << height) + i - 1;
  trit_t *t = mss->mt + MAM2_MSS_MT_HASH_SIZE * idx;
  return trits_from_rep(MAM2_MSS_MT_HASH_SIZE, t);
}
#endif

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

static void mss_fold_auth_path(spongos_t *spongos, mss_mt_idx_t skn,
                               trits_t auth_path, trits_t pk) {
  trits_t hashes[2];

  for (; !trits_is_empty(auth_path);
       skn /= 2, auth_path = trits_drop(auth_path, MAM2_MSS_MT_HASH_SIZE)) {
    hashes[skn % 2] = pk;
    hashes[1 - (skn % 2)] = trits_take(auth_path, MAM2_MSS_MT_HASH_SIZE);
    mss_hash2(spongos, hashes, pk);
  }
}

void mss_init(mss_t *mss, prng_t *prng, sponge_t *sponge, wots_t *wots,
              trint6_t height, trits_t nonce1, trits_t nonce2) {
  MAM2_ASSERT(mss);
  MAM2_ASSERT(prng);
  MAM2_ASSERT(0 <= height && height <= MAM2_MSS_MAX_D);

  mss->height = height;
  mss->skn = 0;
  mss->prng = prng;
  mss->sg->sponge = sponge;
  mss->wots = wots;
  mss->nonce1 = nonce1;
  mss->nonce2 = nonce2;
#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_init(mss);
#endif

#if defined(MAM2_MSS_DEBUG)
  m->gen_leaf_count = m->hash_node_count = 0;
#endif
}

void mss_gen(mss_t *mss, trits_t pk) {
#if defined(MAM2_MSS_TRAVERSAL)
  if (0 == mss->height) {
    mss_mt_gen_leaf(mss, 0, pk);
  } else {
    /* reuse stack `D-1`, by construction (see note in mss.h) */
    /* it has capacity `D+1` */
    mss_mt_height_t height = mss->height - 1;
    mss_mt_stack_t *stack = mss->stacks + MAM2_MSS_MT_STACKS(height);
    mss_mt_node_t *nodes = mss->nodes + MAM2_MSS_MT_NODES(height);
    mss_mt_node_t *node;

    trit_t *nodes_hashes =
        mss->nodes_hashes + MAM2_MSS_MT_HASH_WORDS(height, 0);

    /* init stack */
    /* max node height is `D` */
    stack->height = mss->height;
    /* start leaf index (skn) is `0` */
    stack->level_idx = 0;
    /* empty stack */
    stack->size = 0;

    for (;;) {
      /* update current stack */
      mss_mt_update(mss, height);
      /* top node */
      node = nodes + (stack->size - 1);
      /* is it root? */
      if (node->height == mss->height) { /* done */
        /* copy pk, it is stored outside of stack due to dirty hack */
        trits_t hash = mss_hash_idx(nodes_hashes, stack->size);
        trits_copy(hash, pk);
        /* init stack */
        stack->height = mss->height - 1;
        stack->level_idx = 0;
        stack->size = 1;
        break;
      }

      // This block is for generating the current authentication path
      // and the next one, since this is a `gen` function, skn = 0, current
      // auth path is a path of `mss->height - 1` nodes with `level_idx` = 1
      // i.e (0,1),(1,1)(2,1)...(`mss->height - 1`,1)
      // The next auth path is OTHO the path:
      //(1,1),(1,1)(2,1)...(`mss->height - 1`,1)
      {
        // is it current apath node?
        if (node->level_idx == 1) {
          // add to `auth_path`
          trits_t hash = mss_hash_idx(nodes_hashes, stack->size - 1);
          trits_t auth_path_height_section =
              mss_mt_auth_path_trits(mss, node->height);
          trits_copy(hash, auth_path_height_section);

        }
        // is it next auth_path node?
        else if (node->level_idx == 0 &&
                 (node->height + 1 !=
                  mss->height)) { /* push to stack `n->height` */
          /* stack `n->height` */
          mss_mt_stack_t *current_node_height_stack =
              mss->stacks + MAM2_MSS_MT_STACKS(node->height);
          /* stack `n->height` nodes */
          mss_mt_node_t *current_node_height_nodes =
              mss->nodes + MAM2_MSS_MT_NODES(node->height);
          /* node `n` hash */
          trits_t hash = mss_hash_idx(nodes_hashes, stack->size - 1);
          /* stack `n->height` first node hash */
          trits_t hd = mss_mt_node_hash_trits(mss, node->height, 0);

          MAM2_ASSERT(0 == current_node_height_stack->size);
          /* copy hash */
          trits_copy(hash, hd);
          /* copy node */
          current_node_height_nodes[0] = *node;
          /* increase size */
          current_node_height_stack->size++;
        }
      }
    }
  }

#else
  mss_mt_height_t height;
  mss_mt_idx_t i, n;

  for (i = 0, n = (trint18_t)1 << mss->height; i < n; ++i) {
    trits_t wpk = mss_mt_node_t_trits(mss, mss->height, i);
    mss_mt_gen_leaf(mss, i, wpk);
  }

  for (height = mss->height; height--;) {
    for (i = 0, n = (trint18_t)1 << height; i < n; ++i) {
      trits_t h[2], h01;
      h[0] = mss_mt_node_t_trits(mss, height + 1, 2 * i + 0);
      h[1] = mss_mt_node_t_trits(mss, height + 1, 2 * i + 1);
      h01 = mss_mt_node_t_trits(mss, height, i);

      mss_mt_hash2(mss, h, h01);
    }
  }

  trits_copy(mss_mt_node_t_trits(mss, 0, 0), pk);
#endif
}

void mss_skn(mss_t *mss, trits_t skn) {
  MAM2_TRITS_DEF0(trits, MAM2_MSS_SKN_SIZE);
  trits = MAM2_TRITS_INIT(trits, MAM2_MSS_SKN_SIZE);

  MAM2_ASSERT(trits_size(skn) == MAM2_MSS_SKN_SIZE);

  trits_put6(trits, mss->height);
  trits_copy(trits_take(trits, 4), trits_take(skn, 4));

  trits_put18(trits, mss->skn);
  trits_copy(trits_take(trits, 14), trits_drop(skn, 4));
}

static bool mss_parse_skn(trint6_t *height, trint18_t *skn, trits_t trits) {
  MAM2_TRITS_DEF0(ts, MAM2_MSS_SKN_SIZE);
  ts = MAM2_TRITS_INIT(ts, MAM2_MSS_SKN_SIZE);

  MAM2_ASSERT(MAM2_MSS_SKN_SIZE == trits_size(trits));

  trits_set_zero(ts);

  trits_copy(trits_take(trits, 4), trits_take(ts, 4));
  *height = trits_get6(ts);

  trits_copy(trits_drop(trits, 4), trits_take(ts, 14));
  *skn = trits_get18(ts);

  if (*height < 0 || *skn < 0 || *skn > MAM2_MSS_MAX_SKN(*height)) return 0;

  return 1;
}
void mss_auth_path(mss_t *mss, trint18_t skn, trits_t path) {
  mss_mt_height_t height;
  trits_t path_part_out;

  MAM2_ASSERT(trits_size(path) == MAM2_MSS_APATH_SIZE(mss->height));

#if defined(MAM2_MSS_TRAVERSAL)
  // current apath is already stored in `mss->ap`
  for (height = 0; height < mss->height; ++height)
#else
  /* note, level height is reversed (compared to traversal): */
  /* `0` is root level, `hieght` is leaf level */
  for (height = mss->height; height; --height, skn = skn / 2)
#endif
  {
    path_part_out = trits_take(path, MAM2_MSS_MT_HASH_SIZE);
#if defined(MAM2_MSS_TRAVERSAL)
    trits_t curr_auth_path_part = mss_hash_idx(mss->auth_path, height);
#else
    trits_t curr_auth_path_part =
        mss_mt_node_t_trits(mss, height, (0 == skn % 2) ? skn + 1 : skn - 1);
#endif
    trits_copy(curr_auth_path_part, path_part_out);

    path = trits_drop(path, MAM2_MSS_MT_HASH_SIZE);
  }
}

void mss_sign(mss_t *mss, trits_t hash, trits_t sig) {
  MAM2_ASSERT(trits_size(sig) == MAM2_MSS_SIG_SIZE(mss->height));

  // Write both tree height and the sk index to the signature
  mss_skn(mss, trits_take(sig, MAM2_MSS_SKN_SIZE));
  sig = trits_drop(sig, MAM2_MSS_SKN_SIZE);

#if defined(MAM2_MSS_DEBUG)
  /* instead of WOTS sig gen WOTS pk directly */
  mss_mt_gen_leaf(mss, mss->skn, trits_take(sig, MAM2_MSS_MT_HASH_SIZE));
#else
  {
    // Generate the current (skn) secret key
    MAM2_TRITS_DEF0(nonce_i, MAM2_MSS_SKN_SIZE);
    nonce_i = MAM2_TRITS_INIT(nonce_i, MAM2_MSS_SKN_SIZE);
    trits_put18(nonce_i, mss->skn);
    wots_gen_sk3(mss->wots, mss->prng, mss->nonce1, mss->nonce2, nonce_i);
  }

  wots_sign(mss->wots, hash, trits_take(sig, MAM2_WOTS_SIG_SIZE));
#endif
  sig = trits_drop(sig, MAM2_WOTS_SIG_SIZE);

  mss_auth_path(mss, mss->skn, sig);
}

bool mss_next(mss_t *mss) {
  if (mss->skn == MAM2_MSS_MAX_SKN(mss->height)) return false;

#if defined(MAM2_MSS_TRAVERSAL)
  if (mss->skn < MAM2_MSS_MAX_SKN(mss->height)) {
    mss_mt_refresh(mss);
    mss_mt_build_stacks(mss);
  }
#endif

  mss->skn++;
  return 1;
}

bool mss_verify(spongos_t *mt_spongos, spongos_t *wots_spongos, trits_t hash,
                trits_t sig, trits_t pk) {
  trint6_t height;
  trint18_t skn;
  MAM2_TRITS_DEF0(calculated_pk, MAM2_MSS_MT_HASH_SIZE);
  calculated_pk = MAM2_TRITS_INIT(calculated_pk, MAM2_MSS_MT_HASH_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_MSS_PK_SIZE);

  if (trits_size(sig) < MAM2_MSS_SIG_SIZE(0)) return false;

  if (!mss_parse_skn(&height, &skn, trits_take(sig, MAM2_MSS_SKN_SIZE))) {
    return false;
  }

  sig = trits_drop(sig, MAM2_MSS_SKN_SIZE);
  if (trits_size(sig) != (MAM2_MSS_SIG_SIZE(height) - MAM2_MSS_SKN_SIZE))
    return false;

#if defined(MAM2_MSS_DEBUG)
  trits_copy(trits_take(sig, MAM2_MSS_MT_HASH_SIZE), calculated_pk);
#else
  wots_recover(wots_spongos, hash, trits_take(sig, MAM2_WOTS_SIG_SIZE),
               calculated_pk);
#endif
  sig = trits_drop(sig, MAM2_WOTS_SIG_SIZE);

  mss_fold_auth_path(mt_spongos, skn, sig, calculated_pk);

  return trits_cmp_eq(calculated_pk, pk);
}

retcode_t mss_create(mss_t *mss, mss_mt_height_t height) {
  retcode_t e = RC_OK;
  MAM2_ASSERT(mss);

  memset(mss, 0, sizeof(mss_t));
  ERR_GUARD_RETURN(0 <= height && height <= MAM2_MSS_MAX_D,
                   RC_MAM2_INVALID_ARGUMENT, e);

#if defined(MAM2_MSS_TRAVERSAL)
  mss->auth_path = malloc(sizeof(trit_t) * MAM2_MSS_MT_AUTH_WORDS(height));
  ERR_GUARD_RETURN(mss->auth_path, RC_OOM, e);

  /* add 1 extra hash for dirty hack (see mss.c) */
  mss->nodes_hashes =
      malloc(sizeof(trit_t) * MAM2_MSS_MT_HASH_WORDS(height, 1));
  ERR_GUARD_RETURN(mss->nodes_hashes, RC_OOM, e);

  /* add 1 extra node for dirty hack (see mss.c) */
  mss->nodes = malloc(sizeof(mss_mt_node_t) * (MAM2_MSS_MT_NODES(height) + 1));
  ERR_GUARD_RETURN(mss->nodes, RC_OOM, e);

  mss->stacks = malloc(sizeof(mss_mt_stack_t) * MAM2_MSS_MT_STACKS(height));
  ERR_GUARD_RETURN(mss->nodes, RC_OOM, e);
#else
  mss->mt = malloc(sizeof(trit_t) * MAM2_MSS_MT_WORDS(height));
  ERR_GUARD_RETURN(mss->mt, RC_OOM, e);
#endif

  /* do not free here in case of error */
  return e;
}

void mss_destroy(mss_t *mss) {
  MAM2_ASSERT(mss);

#if defined(MAM2_MSS_TRAVERSAL)
  if (mss->auth_path) {
    free(mss->auth_path), mss->auth_path = 0;
  }
  if (mss->nodes_hashes) {
    free(mss->nodes_hashes), mss->nodes_hashes = 0;
  }
  if (mss->nodes) {
    free(mss->nodes), mss->nodes = 0;
  }
  if (mss->stacks) {
    free(mss->stacks), mss->stacks = 0;
  }
#else
  if (mss->mt) {
    free(mss->mt);
    mss->mt = 0;
  }
#endif
}

static size_t mss_mt_stored_size(mss_t *mss) {
  size_t size = 0;
#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_height_t height;
  size += mss->height * MAM2_MSS_MT_HASH_SIZE;
  for (height = 0; height != mss->height; ++height)
    size += mss->stacks[height].size * MAM2_MSS_MT_HASH_SIZE;
#else
  size += (((size_t)1 << (mss->height + 1)) - 1) * MAM2_MSS_MT_HASH_SIZE;
#endif
  return size;
}

static void mss_mt_save(mss_t *mss, trits_t buffer) {
  mss_mt_height_t height;
  mss_mt_idx_t i;

  MAM2_ASSERT(trits_size(buffer) == mss_mt_stored_size(mss));

#if defined(MAM2_MSS_TRAVERSAL)
  /* <apath> */
  for (height = 0; height != mss->height; ++height) {
    trits_copy(mss_mt_auth_path_trits(mss, height),
               trits_take(buffer, MAM2_MSS_MT_HASH_SIZE));
    buffer = trits_drop(buffer, MAM2_MSS_MT_HASH_SIZE);
  }
  /* <node-hashes> */
  for (height = 0; height != mss->height; ++height) {
    for (i = 0; i != mss->stacks[height].size; ++i) {
      trits_copy(mss_mt_node_hash_trits(mss, height, i),
                 trits_take(buffer, MAM2_MSS_MT_HASH_SIZE));
      buffer = trits_drop(buffer, MAM2_MSS_MT_HASH_SIZE);
    }
  }
#else
  height = mss->height;
  do {
    for (i = 0; i != (mss_mt_idx_t)1 << height; ++i) {
      trits_copy(mss_mt_node_t_trits(mss, height, i),
                 trits_take(buffer, MAM2_MSS_MT_HASH_SIZE));
      buffer = trits_drop(buffer, MAM2_MSS_MT_HASH_SIZE);
    }
  } while (height-- > 0);
#endif
}

/*!
 * \note `mss_mt_rewind` must be called prior `mss_mt_load`.
 */

static void mss_mt_load(mss_t *mss, trits_t buffer) {
  mss_mt_height_t height;
  mss_mt_idx_t i;

  MAM2_ASSERT(trits_size(buffer) == mss_mt_stored_size(mss));

#if defined(MAM2_MSS_TRAVERSAL)
  /* <apath> */
  for (height = 0; height != mss->height; ++height) {
    trits_copy(trits_take(buffer, MAM2_MSS_MT_HASH_SIZE),
               mss_mt_auth_path_trits(mss, height));
    buffer = trits_drop(buffer, MAM2_MSS_MT_HASH_SIZE);
  }
  /* <node-hashes> */
  for (height = 0; height != mss->height; ++height) {
    for (i = 0; i != mss->stacks[height].size; ++i) {
      trits_copy(trits_take(buffer, MAM2_MSS_MT_HASH_SIZE),
                 mss_mt_node_hash_trits(mss, height, i));
      buffer = trits_drop(buffer, MAM2_MSS_MT_HASH_SIZE);
    }
  }
#else
  /* <node-hashes> */
  height = mss->height;
  do {
    for (i = 0; i != (mss_mt_idx_t)1 << height; ++i) {
      trits_copy(trits_take(buffer, MAM2_MSS_MT_HASH_SIZE),
                 mss_mt_node_t_trits(mss, height, i));
      buffer = trits_drop(buffer, MAM2_MSS_MT_HASH_SIZE);
    }
  } while (height-- > 0);
#endif
}

size_t mss_stored_size(mss_t *mss) { return 4 + 14 + mss_mt_stored_size(mss); }

void mss_save(mss_t *mss, trits_t buffer) {
  MAM2_ASSERT(mss_stored_size(mss) == trits_size(buffer));

  mss_skn(mss, trits_take(buffer, MAM2_MSS_SKN_SIZE));
  mss_mt_save(mss, trits_drop(buffer, MAM2_MSS_SKN_SIZE));
}

retcode_t mss_load(mss_t *mss, trits_t *b) {
  retcode_t e = RC_OK;

  mss_mt_height_t height;
  mss_mt_idx_t skn;

  ERR_GUARD_RETURN(4 + 14 <= trits_size(*b), RC_MAM2_BUFFER_TOO_SMALL, e);
  ERR_GUARD_RETURN(mss_parse_skn(&height, &skn, trits_advance(b, 4 + 14)),
                   RC_MAM2_INVALID_VALUE, e);
  ERR_GUARD_RETURN(height == mss->height, RC_MAM2_INVALID_VALUE, e);

#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_rewind(mss, skn);
#else
  mss->skn = skn;
#endif
  ERR_GUARD_RETURN(mss_mt_stored_size(mss) <= trits_size(*b),
                   RC_MAM2_BUFFER_TOO_SMALL, e);
  mss_mt_load(mss, trits_advance(b, mss_mt_stored_size(mss)));

  return e;
}
