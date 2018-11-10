
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
\file buffers.h
\brief Trits collection to simplify buffered operations.
*/
#ifndef __MAM_V2_TEST_STRUCTS_H__
#define __MAM_V2_TEST_STRUCTS_H__

#include "mam/v2/alloc.h"
#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#if !defined(MAM2_MSS_TEST_MAX_D)
#define MAM2_MSS_TEST_MAX_D 3
#endif

typedef struct _test_sponge_s {
  isponge s;
  sponge_state_t stack;
  sponge_state_t state;
} test_sponge_t;

typedef struct _test_prng_s {
  iprng p;
  prng_key_t key;
} test_prng_t;

typedef struct _test_wots_s {
  iwots w;
  wots_sk_t sk;
} test_wots_t;

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss(DEPTH, sfx)                  \
  typedef struct _test_mss##sfx {                 \
    imss m;                                       \
    word_t ap[MAM2_MSS_MT_AUTH_WORDS(DEPTH)];     \
    uint32_t ap_check;                            \
    word_t hs[MAM2_MSS_MT_HASH_WORDS(DEPTH, 1)];  \
    uint32_t hs_check;                            \
    mss_mt_node ns[MAM2_MSS_MT_NODES(DEPTH) + 1]; \
    uint32_t ns_check;                            \
    mss_mt_stack ss[MAM2_MSS_MT_STACKS(DEPTH)];   \
    uint32_t ss_check;                            \
  } test_mss##sfx
#else
#define def_test_mss(D, sfx)         \
  typedef struct _test_mss##sfx {    \
    imss m;                          \
    word_t mt[MAM2_MSS_MT_WORDS(D)]; \
    uint32_t mt_check;               \
  } test_mss##sfx
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_init(DEPTH, sfx)                 \
  static imss *test_mss_init##sfx(test_mss##sfx *m) { \
    m->m.ap = m->ap;                                  \
    m->ap_check = 0xdeadbeef;                         \
    m->m.hs = m->hs;                                  \
    m->hs_check = 0xdeadbeef;                         \
    m->m.ns = m->ns;                                  \
    m->ns_check = 0xdeadbeef;                         \
    m->m.ss = m->ss;                                  \
    m->ss_check = 0xdeadbeef;                         \
    return &m->m;                                     \
  }
#else
#define def_test_mss_init(D, sfx)                               \
  static imss *test_mss_init##sfx(test_mss##sfx *m, iwots *w) { \
    m->m.d = D;                                                 \
    m->m.ws = w;                                                \
    m->m.mt = m->mt;                                            \
    m->mt_check = 0xdeadbeef;                                   \
    return &m->m;                                               \
  }
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_check(D, sfx)                                        \
  static bool_t test_mss_check##sfx(test_mss##sfx *m) {                   \
    return 1 && m->ap_check == 0xdeadbeef && m->hs_check == 0xdeadbeef && \
           m->ns_check == 0xdeadbeef && m->ss_check == 0xdeadbeef;        \
  }
#else
#define def_test_mss_check(D, sfx)                      \
  static bool_t test_mss_check##sfx(test_mss##sfx *m) { \
    return 1 && m->mt_check == 0xdeadbeef;              \
  }
#endif

def_test_mss(1, 1);
def_test_mss(2, 2);
def_test_mss(3, 3);
def_test_mss(4, 4);
def_test_mss(MAM2_MSS_TEST_MAX_D, );

def_test_mss_init(1, 1);
def_test_mss_init(2, 2);
def_test_mss_init(3, 3);
def_test_mss_init(4, 4);
def_test_mss_init(MAM2_MSS_TEST_MAX_D, );

def_test_mss_check(1, 1);
def_test_mss_check(2, 2);
def_test_mss_check(3, 3);
def_test_mss_check(4, 4);
def_test_mss_check(MAM2_MSS_TEST_MAX_D, );

MAM2_SAPI void test_f(void *buf, word_t *s) {
  trits_t x = trits_from_rep(MAM2_SPONGE_RATE, s);
  trits_t y = trits_from_rep(MAM2_SPONGE_RATE, (word_t *)buf);
  trits_t x0 = trits_take(x, MAM2_SPONGE_RATE / 2);
  trits_t x1 = trits_drop(x, MAM2_SPONGE_RATE / 2);
  trits_t x2 =
      trits_drop(trits_from_rep(MAM2_SPONGE_WIDTH, s), MAM2_SPONGE_RATE);

  trits_add(x0, x1, x0);
  trits_add(x0, x2, x0);
  trits_add(x0, x2, x2);

  trits_copy(trits_take(x, MAM2_SPONGE_RATE - 6), trits_drop(y, 6));
  trits_copy(trits_drop(x, MAM2_SPONGE_RATE - 6), trits_take(y, 6));
  trits_copy(y, x);
}

static isponge *test_sponge_init(test_sponge_t *s) {
  s->s.f = test_f;
  s->s.stack = s->stack;
  s->s.s = s->state;
  return &s->s;
}

static iprng *test_prng_init(test_prng_t *p, isponge *s) {
  p->p.s = s;
  p->p.k = p->key;
  return &p->p;
}

static iwots *test_wots_init(test_wots_t *w, isponge *s) {
  w->w.s = s;
  w->w.sk = w->sk;
  return &w->w;
}

#endif  // __MAM_V2_TEST_STRUCTS_H__