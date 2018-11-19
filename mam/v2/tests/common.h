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

#include "common/trinary/add.h"

#if !defined(MAM2_MSS_TEST_MAX_D)
#define MAM2_MSS_TEST_MAX_D 3
#endif

typedef struct _test_sponge_s {
  isponge s;
  sponge_state_t stack;
  sponge_state_t state;
} test_sponge_t;

typedef struct _test_prng_s {
  prng_t p;
  flex_trit_t key[FLEX_TRIT_SIZE_243];
} test_prng_t;

typedef struct _test_wots_s {
  wots_t w;
  wots_sk_t sk;
} test_wots_t;

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss(DEPTH, sfx)                    \
  typedef struct _test_mss##sfx {                   \
    mss_t m;                                        \
    trit_t ap[MAM2_MSS_MT_AUTH_WORDS(DEPTH)];       \
    uint32_t ap_check;                              \
    trit_t hs[MAM2_MSS_MT_HASH_WORDS(DEPTH, 1)];    \
    uint32_t hs_check;                              \
    mss_mt_node_t ns[MAM2_MSS_MT_NODES(DEPTH) + 1]; \
    uint32_t ns_check;                              \
    mss_mt_stack_t ss[MAM2_MSS_MT_STACKS(DEPTH)];   \
    uint32_t ss_check;                              \
  } test_mss##sfx
#else
#define def_test_mss(D, sfx)         \
  typedef struct _test_mss##sfx {    \
    mss_t m;                         \
    trit_t mt[MAM2_MSS_MT_WORDS(D)]; \
    uint32_t mt_check;               \
  } test_mss##sfx
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_init(DEPTH, sfx)                  \
  static mss_t *test_mss_init##sfx(test_mss##sfx *m) { \
    m->m.auth_path = m->ap;                            \
    m->ap_check = 0xdeadbeef;                          \
    m->m.hashes = m->hs;                               \
    m->hs_check = 0xdeadbeef;                          \
    m->m.nodes = m->ns;                                \
    m->ns_check = 0xdeadbeef;                          \
    m->m.stacks = m->ss;                               \
    m->ss_check = 0xdeadbeef;                          \
    return &m->m;                                      \
  }
#else
#define def_test_mss_init(D, sfx)                                 \
  static mss_t *test_mss_init##sfx(test_mss##sfx *m, wots_t *w) { \
    m->m.d = D;                                                   \
    m->m.ws = w;                                                  \
    m->m.mt = m->mt;                                              \
    m->mt_check = 0xdeadbeef;                                     \
    return &m->m;                                                 \
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

// FIXME (@tsvisabo)
#define FLEX_SPONGE_RATE MAM2_SPONGE_RATE

static void xor_trits(trit_t *lhs, trit_t *rhs, trit_t *res, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    res[i] = trit_sum(lhs[i], rhs[i]);
  }
}

void test_f(void *buf, trit_t *s) {
  trit_t x0_trits[MAM2_SPONGE_RATE / 2];
  memcpy(x0_trits, s, MAM2_SPONGE_RATE / 2);
  trit_t x1_trits[MAM2_SPONGE_RATE / 2];
  memcpy(x1_trits, &s[MAM2_SPONGE_RATE / 2], MAM2_SPONGE_RATE / 2);
  trit_t y_trits[MAM2_SPONGE_RATE];
  memcpy(y_trits, buf, MAM2_SPONGE_RATE);

  trit_t x2_trits[MAM2_SPONGE_WIDTH - MAM2_SPONGE_RATE];
  memcpy(x2_trits, &s[MAM2_SPONGE_RATE], MAM2_SPONGE_WIDTH - MAM2_SPONGE_RATE);

  xor_trits(x1_trits, x0_trits, x0_trits, MAM2_SPONGE_RATE / 2);

  xor_trits(x2_trits, x0_trits, x0_trits, MAM2_SPONGE_RATE / 2);

  xor_trits(x0_trits, x2_trits, x2_trits, MAM2_SPONGE_RATE / 2);

  memcpy(s, x0_trits, MAM2_SPONGE_RATE / 2);
  memcpy(&s[MAM2_SPONGE_RATE / 2], x1_trits, MAM2_SPONGE_RATE / 2);
  memcpy(&s[MAM2_SPONGE_RATE], x2_trits, MAM2_SPONGE_WIDTH - MAM2_SPONGE_RATE);
  memcpy(&y_trits[6], s, MAM2_SPONGE_RATE - 6);
  memcpy(y_trits, &s[MAM2_SPONGE_RATE - 6], 6);
  memcpy(s, y_trits, MAM2_SPONGE_RATE);
}

static isponge *test_sponge_init(test_sponge_t *s) {
  s->s.f = test_f;
  s->s.stack = s->stack;
  s->s.s = s->state;
  return &s->s;
}

static prng_t *test_prng_init(test_prng_t *prng, isponge *sponge) {
  prng->p.sponge = sponge;
  memcpy(prng->p.key, prng->key, FLEX_TRIT_SIZE_243);
  return &prng->p;
}

static wots_t *test_wots_init(test_wots_t *w, isponge *s) {
  w->w.sponge = s;
  w->w.sk = w->sk;
  return &w->w;
}

#endif  // __MAM_V2_TEST_STRUCTS_H__
