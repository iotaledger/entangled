
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file mss.c
\brief MAM2 MSS layer.
*/

#include <memory.h>

#include "mam/v2/mss/mss.h"

#define MAM2_MSS_MAX_SKN(d) (((trint18_t)1 << (d)) - 1)

static void mss_hash2(
    ispongos *s,
    trits_t h[2], /*!< [in] hash values of left and right child nodes */
    trits_t h01   /*!< [out] hash value of their parent */
) {
#if defined(MAM2_MSS_DEBUG)
  {
    /* this is a special debug MT hash function */
    mss_mt_idx_t i0, i1, i01;
    /* hash of the node is it's index in level */
    i0 = trits_get18(h[0]);
    /*MAM2_ASSERT(i0 == ns[s->s].i); */
    i1 = trits_get18(h[1]);
    /*MAM2_ASSERT(i1 == ns[s->s+1].i); */
    /* left node is even */
    MAM2_ASSERT(i0 % 2 == 0);
    /* right node is left node + 1 */
    MAM2_ASSERT(i0 + 1 == i1);
    /* parent's index */
    i01 = i0 / 2;
    trits_set_zero(h01);
    trits_put18(h01, i01);
  }
#else
  spongos_hashn(s, 2, h, h01);
#endif
}

static void mss_mt_hash2(imss *m, trits_t h[2], trits_t h01) {
#if defined(MAM2_MSS_DEBUG)
  m->hash_node_count++;
#endif
  mss_hash2(m->sg, h, h01);
}

static void mss_mt_gen_leaf(
    imss *m, mss_mt_idx_t i, /*!< [in] leaf index: `0 <= i < 2^D` */
    trits_t h                /*!< [out] WOTS pk / leaf hash */
) {
#if defined(MAM2_MSS_DEBUG)
  MAM2_ASSERT(0 <= i && i <= MAM2_MSS_MAX_SKN(m->d));

  m->gen_leaf_count++;

  /* this is a special debug MT hash function; */
  /* hash of the node is it's index in the level */
  trits_set_zero(h);
  trits_put18(h, i);
#else
  MAM2_TRITS_DEF0(Ni, 18);

  MAM2_ASSERT(0 <= i && i <= MAM2_MSS_MAX_SKN(m->d));
  Ni = MAM2_TRITS_INIT(Ni, 18);

  /* gen sk from current leaf index */
  trits_put18(Ni, i);
  wots_gen_sk3(m->w, m->p, m->N1, m->N2, Ni);
  /* calc pk & push hash */
  wots_calc_pk(m->w, h);
#endif
}

#if defined(MAM2_MSS_TRAVERSAL)
static trits_t mss_hash_idx(word_t *p, size_t i) {
  return trits_from_rep(MAM2_MSS_MT_HASH_SIZE, p + MAM2_MSS_HASH_IDX(i));
}

static trits_t mss_mt_ap_trits(imss *m, mss_mt_height_t d) {
  return mss_hash_idx(m->ap, d);
}

static trits_t mss_mt_hs_trits(imss *m, mss_mt_height_t d, size_t i) {
  return mss_hash_idx(m->hs, MAM2_MSS_MT_NODES(d) + i);
}

static void mss_mt_init(imss *m) {
  mss_mt_stack *s = m->ss;
  mss_mt_height_t d;

  for (d = 0; d < m->d; d++, s++) {
    s->d = d;
    s->i = 0;
    s->s = 0;
  }
}

static void mss_mt_update(imss *m, mss_mt_height_t d) {
  mss_mt_stack *s;
  mss_mt_node *ns;
  word_t *hs;
  trits_t h[2], wpk;

  /* current level must be lower than MT height */
  MAM2_ASSERT(0 <= d && d < m->d);
  /* stack at level `d` */
  s = m->ss + MAM2_MSS_MT_STACKS(d);
  /* stack's nodes */
  ns = m->ns + MAM2_MSS_MT_NODES(d);
  /* stack's hashes stored separately from nodes */
  hs = m->hs + MAM2_MSS_MT_HASH_WORDS(d, 0);

  /* finished? */
  if ((0 != s->s) && (ns[s->s - 1].d >= s->d)) return;

  /* can merge (top 2 nodes have the same height)? */
  if (s->s > 1 && (ns[s->s - 2].d == ns[s->s - 1].d)) { /* merge */
    /* pop the right node */
    h[1] = mss_hash_idx(hs, --s->s);
    /* pop the left node */
    h[0] = mss_hash_idx(hs, --s->s);

    /* hash them */
    /* dirty hack: at the last level do not overwrite  */
    /* left hash value, but instead right; */
    /* left hash value is needed for MTT algorithm */
    mss_mt_hash2(m, h, h[ns[s->s].d + 1 != m->d ? 0 : 1]);

    /* push parent into stack */
    /* parent is one level up */
    ns[s->s].d += 1;
    /* parent's index */
    ns[s->s].i /= 2;
    /* adjust stack size */
    s->s++;
  } else if (s->i <= MAM2_MSS_MAX_SKN(m->d)) {
    /* pk will be put on top of the stack */
    wpk = mss_hash_idx(hs, s->s);
    mss_mt_gen_leaf(m, s->i, wpk);

    /* push leaf into stack */
    /* leaf has level `0` */
    ns[s->s].d = 0;
    /* leaf's index */
    ns[s->s].i = s->i;
    /* increase stack size */
    s->s++;
    /* increment leaf index (skn) */
    s->i++;
  }
}

static void mss_mt_load_update(imss *m, mss_mt_height_t d) {
  mss_mt_stack *s;
  mss_mt_node *ns;

  /* current level must be lower than MT height */
  MAM2_ASSERT(0 <= d && d < m->d);
  /* stack at level `d` */
  s = m->ss + MAM2_MSS_MT_STACKS(d);
  /* stack's nodes */
  ns = m->ns + MAM2_MSS_MT_NODES(d);

  /* finished? */
  if ((0 != s->s) && (ns[s->s - 1].d >= s->d)) return;

  /* can merge (top 2 nodes have the same height)? */
  if (s->s > 1 && (ns[s->s - 2].d == ns[s->s - 1].d)) { /* merge */
    /* pop the right node */
    --s->s;
    /* pop the left node */
    --s->s;

    /* hash them */

    /* push parent into stack */
    /* parent is one level up */
    ns[s->s].d += 1;
    /* parent's index */
    ns[s->s].i /= 2;
    /* adjust stack size */
    s->s++;
  } else if (s->i <= MAM2_MSS_MAX_SKN(m->d)) {
    /* pk will be put on top of the stack */

    /* push leaf into stack */
    /* leaf has level `0` */
    ns[s->s].d = 0;
    /* leaf's index */
    ns[s->s].i = s->i;
    /* increase stack size */
    s->s++;
    /* increment leaf index (skn) */
    s->i++;
  }
}

static void mss_mt_refresh(imss *m) {
  mss_mt_stack *s;
  mss_mt_height_t d;
  mss_mt_idx_t dd;

  for (d = 0; d < m->d; d++) {
    dd = (mss_mt_idx_t)1 << d;
    if ((m->skn + 1) % dd != 0) break;

    s = m->ss + d;
    MAM2_ASSERT(s->s == 1);
    trits_copy(mss_mt_hs_trits(m, d, s->s - 1), mss_mt_ap_trits(m, d));

    s->i = (m->skn + 1 + dd) ^ dd;
    s->d = d;
    s->s = 0;
  }
}

static void mss_mt_load_refresh(imss *m) {
  mss_mt_stack *s;
  mss_mt_height_t d;
  mss_mt_idx_t dd;

  for (d = 0; d < m->d; d++) {
    dd = (mss_mt_idx_t)1 << d;
    if ((m->skn + 1) % dd != 0) break;

    s = m->ss + d;
    MAM2_ASSERT(s->s == 1);

    s->i = (m->skn + 1 + dd) ^ dd;
    s->d = d;
    s->s = 0;
  }
}

static void mss_mt_build_stacks(imss *m) {
  /* classic Merkle tree traversal variant */
  mss_mt_height_t d;
  for (d = 0; d < m->d; ++d) mss_mt_update(m, d), mss_mt_update(m, d);
}

static void mss_mt_load_build_stacks(imss *m) {
  mss_mt_height_t d;
  for (d = 0; d < m->d; ++d) mss_mt_load_update(m, d), mss_mt_load_update(m, d);
}

void mss_mt_rewind(imss *m, trint18_t skn) {
  mss_mt_stack *s;
  mss_mt_node *ns;
  mss_mt_height_t d;

  for (d = 0; d < m->d; ++d) {
    s = m->ss + MAM2_MSS_MT_STACKS(d);
    ns = m->ns + MAM2_MSS_MT_NODES(d);

    s->d = d;
    s->i = 0;
    s->s = 1;
    ns->d = d;
    ns->i = 0;
  }
  if (0 < m->d) ns->d++;

  for (m->skn = 0; m->skn < skn; m->skn++) {
    mss_mt_load_refresh(m);
    mss_mt_load_build_stacks(m);
  }
}
#else
static trits_t mss_mt_node_trits(imss *m, trint6_t d, trint18_t i) {
  MAM2_ASSERT(d <= m->d);
  MAM2_ASSERT(i < ((trint18_t)1 << d));

  size_t idx = ((size_t)1 << d) + i - 1;
  word_t *w = m->mt + MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE) * idx;
  return trits_from_rep(MAM2_MSS_MT_HASH_SIZE, w);
}
#endif

static void mss_fold_apath(
    ispongos *s,
    mss_mt_idx_t skn, /*!< [in] corresponding WOTS sk number / leaf index */
    trits_t ap,       /*!< [in] authentication path - leaf to root */
    trits_t h         /*!< [in] recovered WOTS pk / start hash value;
                           [out] recovered MT root */
) {
  trits_t t[2];

  for (; !trits_is_empty(ap);
       skn /= 2, ap = trits_drop(ap, MAM2_MSS_MT_HASH_SIZE)) {
    t[skn % 2] = h;
    t[1 - (skn % 2)] = trits_take(ap, MAM2_MSS_MT_HASH_SIZE);
    mss_hash2(s, t, h);
  }
}

void mss_init(imss *m, iprng *p, isponge *s, iwots *w, trint6_t d, trits_t N1,
              trits_t N2) {
  MAM2_ASSERT(m);
  MAM2_ASSERT(p);
  MAM2_ASSERT(0 <= d && d <= MAM2_MSS_MAX_D);

  m->d = d;
  m->skn = 0;
  m->p = p;
  m->sg->s = s;
  m->w = w;
  m->N1 = N1;
  m->N2 = N2;
#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_init(m);
#endif

#if defined(MAM2_MSS_DEBUG)
  m->gen_leaf_count = m->hash_node_count = 0;
#endif
}

void mss_gen(imss *m, trits_t pk) {
#if defined(MAM2_MSS_TRAVERSAL)
  if (0 == m->d)
    mss_mt_gen_leaf(m, 0, pk);
  else {
    /* reuse stack `D-1`, by construction (see note in mss.h) */
    /* it has capacity `D+1` */
    mss_mt_height_t d = m->d - 1;
    mss_mt_stack *s = m->ss + MAM2_MSS_MT_STACKS(d);
    mss_mt_node *ns = m->ns + MAM2_MSS_MT_NODES(d), *n;
    word_t *hs = m->hs + MAM2_MSS_MT_HASH_WORDS(d, 0);

    /* init stack */
    /* max node height is `D` */
    s->d = m->d;
    /* start leaf index (skn) is `0` */
    s->i = 0;
    /* empty stack */
    s->s = 0;

    for (;;) {
      /* update current stack */
      mss_mt_update(m, d);

      /* top node */
      n = ns + (s->s - 1);
      /* is it root? */
      if (n->d == m->d) { /* done */
        /* copy pk, it is stored outside of stack due to dirty hack */
        trits_t h = mss_hash_idx(hs, s->s);
        trits_copy(h, pk);
        /* init stack */
        s->d = m->d - 1;
        s->i = 0;
        s->s = 1;
        break;
      }

      /* is it current apath node? */
      if (n->i == 1) { /* add to `ap` */
        trits_t h = mss_hash_idx(hs, s->s - 1);
        trits_t a = mss_mt_ap_trits(m, n->d);
        trits_copy(h, a);
      } else

          /* is it next apath node? */
          if (n->i == 0 && (n->d + 1 != m->d)) { /* push to stack `n->d` */
        /* stack `n->d` */
        mss_mt_stack *sd = m->ss + MAM2_MSS_MT_STACKS(n->d);
        /* stack `n->d` nodes */
        mss_mt_node *nsd = m->ns + MAM2_MSS_MT_NODES(n->d);
        /* node `n` hash */
        trits_t h = mss_hash_idx(hs, s->s - 1);
        /* stack `n->d` first node hash */
        trits_t hd = mss_mt_hs_trits(m, n->d, 0);

        MAM2_ASSERT(0 == sd->s);
        /* copy hash */
        trits_copy(h, hd);
        /* copy node */
        nsd[0] = *n;
        /* increase size */
        sd->s++;
      }
    }
  }

#else
  mss_mt_height_t d;
  mss_mt_idx_t i, n;

  for (i = 0, n = (trint18_t)1 << m->d; i < n; ++i) {
    trits_t wpk = mss_mt_node_trits(m, m->d, i);
    mss_mt_gen_leaf(m, i, wpk);
  }

  for (d = m->d; d--;) {
    for (i = 0, n = (trint18_t)1 << d; i < n; ++i) {
      trits_t h[2], h01;
      h[0] = mss_mt_node_trits(m, d + 1, 2 * i + 0);
      h[1] = mss_mt_node_trits(m, d + 1, 2 * i + 1);
      h01 = mss_mt_node_trits(m, d, i);

      mss_mt_hash2(m, h, h01);
    }
  }

  trits_copy(mss_mt_node_trits(m, 0, 0), pk);
#endif
}

void mss_skn(imss *m, trits_t skn) {
  MAM2_TRITS_DEF0(ts, 18);
  ts = MAM2_TRITS_INIT(ts, 18);

  MAM2_ASSERT(trits_size(skn) == MAM2_MSS_SKN_SIZE);

  trits_put6(ts, m->d);
  trits_copy(trits_take(ts, 4), trits_take(skn, 4));

  trits_put18(ts, m->skn);
  trits_copy(trits_take(ts, 14), trits_drop(skn, 4));
}

static bool_t mss_parse_skn(trint6_t *d, trint18_t *skn, trits_t t) {
  MAM2_TRITS_DEF0(ts, 18);
  ts = MAM2_TRITS_INIT(ts, 18);

  MAM2_ASSERT(18 == trits_size(t));

  trits_set_zero(ts);

  trits_copy(trits_take(t, 4), trits_take(ts, 4));
  *d = trits_get6(ts);

  trits_copy(trits_drop(t, 4), trits_take(ts, 14));
  *skn = trits_get18(ts);

  if (*d < 0 || *skn < 0 || *skn > MAM2_MSS_MAX_SKN(*d)) return 0;

  return 1;
}

void mss_apath(imss *m, trint18_t i, trits_t p) {
  mss_mt_height_t d;

  MAM2_ASSERT(trits_size(p) == MAM2_MSS_APATH_SIZE(m->d));

#if defined(MAM2_MSS_TRAVERSAL)
  /*current apath is already stored in `m->ap` */
  for (d = 0; d < m->d; ++d)
#else
  /* note, level height is reversed (compared to traversal): */
  /* `0` is root level, `D` is leaf level */
  for (d = m->d; d; --d, i = i / 2)
#endif
  {
    trits_t pi = trits_take(p, MAM2_MSS_MT_HASH_SIZE);
#if defined(MAM2_MSS_TRAVERSAL)
    trits_t ni = mss_hash_idx(m->ap, d);
#else
    trits_t ni = mss_mt_node_trits(m, d, (0 == i % 2) ? i + 1 : i - 1);
#endif
    trits_copy(ni, pi);

    p = trits_drop(p, MAM2_MSS_MT_HASH_SIZE);
  }
}

void mss_sign(imss *m, trits_t H, trits_t sig) {
  MAM2_ASSERT(trits_size(sig) == MAM2_MSS_SIG_SIZE(m->d));

  mss_skn(m, trits_take(sig, MAM2_MSS_SKN_SIZE));
  sig = trits_drop(sig, MAM2_MSS_SKN_SIZE);

#if defined(MAM2_MSS_DEBUG)
  /* instead of WOTS sig gen WOTS pk directly */
  mss_mt_gen_leaf(m, m->skn, trits_take(sig, MAM2_MSS_MT_HASH_SIZE));
#else
  {
    MAM2_TRITS_DEF0(Ni, 18);
    Ni = MAM2_TRITS_INIT(Ni, 18);
    trits_put18(Ni, m->skn);
    wots_gen_sk3(m->w, m->p, m->N1, m->N2, Ni);
  }

  wots_sign(m->w, H, trits_take(sig, MAM2_WOTS_SIG_SIZE));
#endif
  sig = trits_drop(sig, MAM2_WOTS_SIG_SIZE);

  mss_apath(m, m->skn, sig);
}

bool_t mss_next(imss *m) {
  if (m->skn == MAM2_MSS_MAX_SKN(m->d)) return 0;

#if defined(MAM2_MSS_TRAVERSAL)
  if (m->skn < MAM2_MSS_MAX_SKN(m->d)) {
    mss_mt_refresh(m);
    mss_mt_build_stacks(m);
  }
#endif

  m->skn++;
  return 1;
}

bool_t mss_verify(ispongos *ms, ispongos *ws, trits_t H, trits_t sig,
                  trits_t pk) {
  trint6_t d;
  trint18_t skn;
  MAM2_TRITS_DEF0(apk, MAM2_MSS_MT_HASH_SIZE);
  apk = MAM2_TRITS_INIT(apk, MAM2_MSS_MT_HASH_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_MSS_PK_SIZE);

  if (trits_size(sig) < MAM2_MSS_SIG_SIZE(0)) return 0;

  if (!mss_parse_skn(&d, &skn, trits_take(sig, 18))) return 0;

  sig = trits_drop(sig, 18);
  if (trits_size(sig) != (MAM2_MSS_SIG_SIZE(d) - MAM2_MSS_SKN_SIZE)) return 0;

#if defined(MAM2_MSS_DEBUG)
  trits_copy(trits_take(sig, MAM2_MSS_MT_HASH_SIZE), apk);
#else
  wots_recover(ws, H, trits_take(sig, MAM2_WOTS_SIG_SIZE), apk);
#endif
  sig = trits_drop(sig, MAM2_WOTS_SIG_SIZE);

  mss_fold_apath(ms, skn, sig, apk);

  return trits_cmp_eq(apk, pk);
}

err_t mss_create(ialloc *a, imss *m, mss_mt_height_t d) {
  err_t e = err_internal_error;
  MAM2_ASSERT(m);

  do {
    memset(m, 0, sizeof(imss));
    err_guard(0 <= d && d <= MAM2_MSS_MAX_D, err_invalid_argument);

#if defined(MAM2_MSS_TRAVERSAL)
    m->ap = mam_words_alloc(a, MAM2_MSS_MT_AUTH_WORDS(d));
    err_guard(m->ap, err_bad_alloc);

    /* add 1 extra hash for dirty hack (see mss.c) */
    m->hs = mam_words_alloc(a, MAM2_MSS_MT_HASH_WORDS(d, 1));
    err_guard(m->hs, err_bad_alloc);

    /* add 1 extra node for dirty hack (see mss.c) */
    m->ns = mam_alloc(a, sizeof(mss_mt_node) * (MAM2_MSS_MT_NODES(d) + 1));
    err_guard(m->ns, err_bad_alloc);

    m->ss = mam_alloc(a, sizeof(mss_mt_stack) * MAM2_MSS_MT_STACKS(d));
    err_guard(m->ns, err_bad_alloc);
#else
    m->mt = mam_words_alloc(a, MAM2_MSS_MT_WORDS(d));
    err_guard(m->mt, err_bad_alloc);
#endif

    e = err_ok;
  } while (0);

  /* do not free here in case of error */
  return e;
}

void mss_destroy(ialloc *a, imss *m) {
  MAM2_ASSERT(m);

#if defined(MAM2_MSS_TRAVERSAL)
  if (m->ap) {
    mam_words_free(a, m->ap), m->ap = 0;
  }
  if (m->hs) {
    mam_words_free(a, m->hs), m->hs = 0;
  }
  if (m->ns) {
    mam_free(a, m->ns), m->ns = 0;
  }
  if (m->ss) {
    mam_free(a, m->ss), m->ss = 0;
  }
#else
  if (m->mt) {
    mam_words_free(a, m->mt);
    m->mt = 0;
  }
#endif
}

static size_t mss_mt_stored_size(imss *m) {
  size_t s = 0;
#if defined(MAM2_MSS_TRAVERSAL)
  mss_mt_height_t d;
  s += m->d * MAM2_MSS_MT_HASH_SIZE;
  for (d = 0; d != m->d; ++d) s += m->ss[d].s * MAM2_MSS_MT_HASH_SIZE;
#else
  s += ((size_t)1 << m->d) * MAM2_MSS_MT_HASH_SIZE;
#endif
  return s;
}

static void mss_mt_save(imss *m, trits_t b) {
  mss_mt_height_t d;
  mss_mt_idx_t i;

  MAM2_ASSERT(trits_size(b) == mss_mt_stored_size(m));

#if defined(MAM2_MSS_TRAVERSAL)
  /* <apath> */
  for (d = 0; d != m->d; ++d) {
    trits_copy(mss_mt_ap_trits(m, d), trits_take(b, MAM2_MSS_MT_HASH_SIZE));
    b = trits_drop(b, MAM2_MSS_MT_HASH_SIZE);
  }
  /* <node-hashes> */
  for (d = 0; d != m->d; ++d) {
    for (i = 0; i != m->ss[d].s; ++i) {
      trits_copy(mss_mt_hs_trits(m, d, i),
                 trits_take(b, MAM2_MSS_MT_HASH_SIZE));
      b = trits_drop(b, MAM2_MSS_MT_HASH_SIZE);
    }
  }
#else
  /* <node-hashes> */
  for (d = 0; d != m->d; ++d) {
    for (i = 0; i != m->ss[d].s; ++i) {
      trits_copy(mss_mt_hs_trits(m, d, i),
                 trits_take(b, MAM2_MSS_MT_HASH_SIZE));
      b = trits_drop(b, MAM2_MSS_MT_HASH_SIZE);
    }
  }
#endif
}

/*! \note `mss_mt_rewind` must be called prior `mss_mt_load`. */
static void mss_mt_load(imss *m, trits_t b) {
  mss_mt_height_t d;
  mss_mt_idx_t i;

  MAM2_ASSERT(trits_size(b) == mss_mt_stored_size(m));

#if defined(MAM2_MSS_TRAVERSAL)
  /* <apath> */
  for (d = 0; d != m->d; ++d) {
    trits_copy(trits_take(b, MAM2_MSS_MT_HASH_SIZE), mss_mt_ap_trits(m, d));
    b = trits_drop(b, MAM2_MSS_MT_HASH_SIZE);
  }
  /* <node-hashes> */
  for (d = 0; d != m->d; ++d) {
    for (i = 0; i != m->ss[d].s; ++i) {
      trits_copy(trits_take(b, MAM2_MSS_MT_HASH_SIZE),
                 mss_mt_hs_trits(m, d, i));
      b = trits_drop(b, MAM2_MSS_MT_HASH_SIZE);
    }
  }
#else
  /* <node-hashes> */
  for (d = 0; d != m->d; ++d) {
    for (i = 0; i != m->ss[d].s; ++i) {
      trits_copy(trits_take(b, MAM2_MSS_MT_HASH_SIZE),
                 mss_mt_hs_trits(m, d, i));
      b = trits_drop(b, MAM2_MSS_MT_HASH_SIZE);
    }
  }
#endif
}

size_t mss_stored_size(imss *m) { return 4 + 14 + mss_mt_stored_size(m); }

void mss_save(imss *m, trits_t b) {
  MAM2_ASSERT(mss_stored_size(m) == trits_size(b));

  mss_skn(m, trits_take(b, 18));
  mss_mt_save(m, trits_drop(b, 18));
}

err_t mss_load(imss *m, trits_t *b) {
  err_t e = err_internal_error;

  mss_mt_height_t d;
  mss_mt_idx_t skn;

  do {
    err_guard(4 + 14 <= trits_size(*b), err_buffer_too_small);
    err_guard(mss_parse_skn(&d, &skn, trits_advance(b, 4 + 14)),
              err_invalid_value);
    err_guard(d == m->d, err_invalid_value);

#if defined(MAM2_MSS_TRAVERSAL)
    mss_mt_rewind(m, skn);
#endif
    err_guard(mss_mt_stored_size(m) <= trits_size(*b), err_buffer_too_small);
    mss_mt_load(m, trits_advance(b, mss_mt_stored_size(m)));

    e = err_ok;
  } while (0);

  return e;
}
