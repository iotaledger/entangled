/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 [ITSec Lab]

 *
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file prng.c
\brief MAM2 PRNG layer.
*/
#include "mam/v2/prng.h"

static void prng_absorbn(isponge *s, size_t n, trits_t *KdN) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_KEY, n, KdN);
}

static void prng_squeeze(isponge *s, trits_t Y) {
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, Y);
}

MAM2_INLINE static trits_t prng_key_trits(iprng *p) {
  return trits_from_rep(MAM2_PRNG_KEY_SIZE, p->k);
}

MAM2_SAPI void prng_init(iprng *p, isponge *s, trits_t K) {
  MAM2_ASSERT(trits_size(K) == MAM2_PRNG_KEY_SIZE);

  p->s = s;
  trits_copy(K, prng_key_trits(p));
}

MAM2_SAPI void prng_gen(iprng *p, trint3_t d, trits_t N, trits_t Y) {
  MAM2_TRITS_DEF(dt, 3);
  trits_t KdN[3] = {prng_key_trits(p), dt, N};
  trits_put3(dt, d);

  flex_trit_t p_flex[NUM_FLEX_TRITS_FOR_TRITS(MAM2_PRNG_KEY_SIZE)];
  flex_trit_t dt_flex[dt.n - dt.d];
  flex_trit_t N_flex[N.n - N.d];
  flex_from_trits(dt, dt_flex);
  flex_from_trits(N, N_flex);
  flex_from_trits(prng_key_trits(p), p_flex);
  flex_trit_t *kdn_trits[3] = {p_flex, dt_flex, N_flex};
  prng_absorbn(p->s, 3, KdN);
  prng_squeeze(p->s, Y);
}

MAM2_SAPI void prng_gen2(iprng *p, trint3_t d, trits_t N1, trits_t N2,
                         trits_t Y) {
  MAM2_TRITS_DEF(dt, 3);
  trits_t KdN[4] = {prng_key_trits(p), dt, N1, N2};
  trits_put3(dt, d);

  flex_trit_t p_flex[NUM_FLEX_TRITS_FOR_TRITS(MAM2_PRNG_KEY_SIZE)];
  flex_trit_t dt_flex[dt.n - dt.d];
  flex_trit_t N1_flex[N1.n - N1.d];
  flex_trit_t N2_flex[N2.n - N2.d];
  flex_from_trits(dt, dt_flex);
  flex_from_trits(N1, N1_flex);
  flex_from_trits(N2, N2_flex);
  flex_from_trits(prng_key_trits(p), p_flex);
  flex_trit_t *kdn_trits[4] = {p_flex, dt_flex, N1_flex, N2_flex};

  prng_absorbn(p->s, 4, KdN);
  prng_squeeze(p->s, Y);
}

MAM2_SAPI void prng_gen3(iprng *p, trint3_t d, trits_t N1, trits_t N2,
                         trits_t N3, trits_t Y) {
  memset(Y.p, 0, Y.n);
  MAM2_TRITS_DEF(dt, 3);
  trits_t KdN[5] = {prng_key_trits(p), dt, N1, N2, N3};
  trits_put3(dt, d);

  flex_trit_t p_flex[NUM_FLEX_TRITS_FOR_TRITS(MAM2_PRNG_KEY_SIZE)];
  flex_trit_t dt_flex[dt.n - dt.d];
  flex_trit_t N1_flex[N1.n - N1.d];
  flex_trit_t N2_flex[N2.n - N2.d];
  flex_trit_t N3_flex[N3.n - N3.d];
  flex_from_trits(dt, dt_flex);
  flex_from_trits(N1, N1_flex);
  flex_from_trits(N2, N2_flex);
  flex_from_trits(prng_key_trits(p), p_flex);
  flex_trit_t *kdn_trits[5] = {p_flex, dt_flex, N1_flex, N2_flex, N3_flex};
  prng_absorbn(p->s, 5, KdN);
  prng_squeeze(p->s, Y);
}

MAM2_SAPI err_t prng_create(ialloc *a, iprng *p) {
  err_t e = err_internal_error;
  MAM2_ASSERT(p);
  do {
    memset(p, 0, sizeof(iprng));
    p->k = mam2_words_alloc(a, MAM2_WORDS(MAM2_PRNG_KEY_SIZE));
    err_guard(p->k, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

MAM2_SAPI void prng_destroy(ialloc *a, iprng *p) {
  MAM2_ASSERT(p);
  mam2_words_free(a, p->k);
  p->k = 0;
}
