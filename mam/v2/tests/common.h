/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licenodesing information
 */

#ifndef __MAM_V2_TEST_STRUCTS_H__
#define __MAM_V2_TEST_STRUCTS_H__

#include "common/trinary/add.h"
#include "mam/v2/mss.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#if !defined(MAM2_MSS_TEST_MAX_D)
#define MAM2_MSS_TEST_MAX_D 3
#endif

typedef struct _test_sponge_s {
  sponge_t s;
  sponge_state_t stack;
  sponge_state_t state;
} test_sponge_t;

typedef struct _test_prng_s {
  prng_t p;
  flex_trit_t key[MAM2_PRNG_KEY_FLEX_SIZE];
} test_prng_t;

typedef struct _test_wots_s {
  wots_t w;
  flex_trit_t sk[MAM2_WOTS_SK_FLEX_SIZE];
} test_wots_t;

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss(DEPTH, sfx)                                \
  typedef struct _test_mss##sfx {                               \
    mss_t m;                                                    \
    flex_trit_t auth_path[MAM2_MSS_AUTH_PATH_FLEX_SIZE(DEPTH)]; \
    uint32_t auth_path_check;                                   \
    trit_t hashes[MAM2_MSS_MT_HASH_WORDS(DEPTH, 1)];            \
    uint32_t hashes_check;                                      \
    mss_mt_node_t nodes[MAM2_MSS_MT_NODES(DEPTH) + 1];          \
    uint32_t nodes_check;                                       \
    mss_mt_stack_t stacks[MAM2_MSS_MT_STACKS(DEPTH)];           \
    uint32_t stacks_check;                                      \
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
    m->m.auth_path = m->auth_path;                     \
    m->auth_path_check = 0xdeadbeef;                   \
    m->m.hashes = m->hashes;                           \
    m->hashes_check = 0xdeadbeef;                      \
    m->m.nodes = m->nodes;                             \
    m->nodes_check = 0xdeadbeef;                       \
    m->m.stacks = m->stacks;                           \
    m->stacks_check = 0xdeadbeef;                      \
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
#define def_test_mss_check(D, sfx)                                          \
  static bool test_mss_check##sfx(test_mss##sfx *m) {                       \
    return m->auth_path_check == 0xdeadbeef &&                              \
           m->hashes_check == 0xdeadbeef && m->nodes_check == 0xdeadbeef && \
           m->stacks_check == 0xdeadbeef;                                   \
  }
#else
#define def_test_mss_check(D, sfx)                    \
  static bool test_mss_check##sfx(test_mss##sfx *m) { \
    return m->mt_check == 0xdeadbeef;                 \
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

static sponge_t *test_sponge_init(test_sponge_t *s) {
  s->s.f = test_f;
  s->s.stack = s->stack;
  s->s.s = s->state;
  return &s->s;
}

static prng_t *test_prng_init(test_prng_t *prng, sponge_t *sponge) {
  prng->p.sponge = sponge;
  memcpy(prng->p.key, prng->key, MAM2_PRNG_KEY_FLEX_SIZE);
  return &prng->p;
}

static wots_t *test_wots_init(test_wots_t *w, sponge_t *s) {
  w->w.sponge = s;
  memcpy(w->w.sk, w->sk, MAM2_WOTS_SK_FLEX_SIZE);
  return &w->w;
}

#endif  // __MAM_V2_TEST_STRUCTS_H__
