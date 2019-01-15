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

#include "mam/v2/mss/mss.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/poly.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/wots/wots.h"

typedef struct test_ntru_s {
  intru n;
  word_t id[MAM2_WORDS(81)];
  word_t sk[MAM2_WORDS(MAM2_NTRU_SK_SIZE)];
  poly_t f;
} test_ntru_t;

typedef struct test_prng_s {
  iprng p;
  prng_key_t key;
} test_prng_t;

typedef struct test_sponge_s {
  isponge s;
  sponge_state_t stack;
  sponge_state_t state;
} test_sponge_t;

typedef ispongos test_spongos_t;

typedef struct test_wots_s {
  iwots w;
  wots_sk_t sk;
} test_wots_t;

intru *test_ntru_init(test_ntru_t *n);
iprng *test_prng_init(test_prng_t *p, isponge *s);
isponge *test_sponge_init(test_sponge_t *s);
ispongos *test_spongos_init(test_spongos_t *sg, isponge *s);
iwots *test_wots_init(test_wots_t *w, isponge *s);

#endif  // __MAM_V2_TEST_UTILS_TEST_UTILS_H__
