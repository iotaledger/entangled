/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_TEST_UTILS_TEST_UTILS_H__
#define __MAM_V2_TEST_UTILS_TEST_UTILS_H__

#include <stdlib.h>

#include "mam/v2/mss/mss.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/poly.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MAM2_MSS_TEST_MAX_D)
#define MAM2_MSS_TEST_MAX_D 3
#endif

typedef struct test_ntru_s {
  ntru_t n;
  word_t id[MAM2_WORDS(81)];
  word_t sk[MAM2_WORDS(MAM2_NTRU_SK_SIZE)];
  poly_t f;
} test_ntru_t;

typedef struct test_prng_s {
  prng_t p;
  prng_key_t key;
} test_prng_t;

typedef struct test_sponge_s {
  sponge_t s;
  sponge_state_t stack;
  sponge_state_t state;
} test_sponge_t;

typedef spongos_t test_spongos_t;

typedef struct test_wots_s {
  wots_t w;
  wots_sk_t sk;
} test_wots_t;

ntru_t *test_ntru_init(test_ntru_t *n);
prng_t *test_prng_init(test_prng_t *p, sponge_t *s);
sponge_t *test_sponge_init(test_sponge_t *s);
spongos_t *test_spongos_init(test_spongos_t *sg, sponge_t *s);
wots_t *test_wots_init(test_wots_t *w, sponge_t *s);

static inline sponge_t *test_create_sponge() {
  test_sponge_t *t = malloc(sizeof(test_sponge_t));
  return test_sponge_init(t);
}
static inline void test_delete_sponge(sponge_t *s) { free((test_sponge_t *)s); }

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_TEST_UTILS_TEST_UTILS_H__
