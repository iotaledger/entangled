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

#include <stdlib.h>

#include "mam/v2/prng.h"

/*
 * Private functions
 */

static void prng_absorbn(isponge *s, size_t n, trits_t *KdN) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_KEY, n, KdN);
}

static void prng_squeeze(isponge *s, trits_t Y) {
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, Y);
}

static trits_t prng_key_trits(prng_t *prng) {
  return trits_from_rep(MAM2_PRNG_KEY_SIZE, prng->key);
}

/*
 * Public functions
 */

err_t prng_create(prng_t *prng) {
  err_t e = err_internal_error;
  MAM2_ASSERT(prng);
  do {
    memset(prng, 0, sizeof(prng_t));
    prng->key =
        (trit_t *)malloc(sizeof(trit_t) * MAM2_WORDS(MAM2_PRNG_KEY_SIZE));
    err_guard(prng->key, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

void prng_init(prng_t *prng, isponge *s, trits_t K) {
  MAM2_ASSERT(trits_size(K) == MAM2_PRNG_KEY_SIZE);

  prng->sponge = s;
  trits_copy(K, prng_key_trits(prng));
}

void prng_gen(prng_t *prng, trint3_t d, trits_t N, trits_t Y) {
  MAM2_TRITS_DEF(dt, 3);
  trits_t KdN[3] = {prng_key_trits(prng), dt, N};
  trits_put3(dt, d);
  prng_absorbn(prng->sponge, 3, KdN);
  prng_squeeze(prng->sponge, Y);
}

void prng_gen2(prng_t *prng, trint3_t d, trits_t N1, trits_t N2, trits_t Y) {
  MAM2_TRITS_DEF(dt, 3);
  trits_t KdN[4] = {prng_key_trits(prng), dt, N1, N2};
  trits_put3(dt, d);
  prng_absorbn(prng->sponge, 4, KdN);
  prng_squeeze(prng->sponge, Y);
}

void prng_gen3(prng_t *prng, trint3_t d, trits_t N1, trits_t N2, trits_t N3,
               trits_t Y) {
  memset(Y.p, 0, Y.n);
  MAM2_TRITS_DEF(dt, 3);
  trits_t KdN[5] = {prng_key_trits(prng), dt, N1, N2, N3};
  trits_put3(dt, d);
  prng_absorbn(prng->sponge, 5, KdN);
  prng_squeeze(prng->sponge, Y);
}

void prng_destroy(prng_t *prng) {
  MAM2_ASSERT(prng);
  free(prng->key);
  prng->key = 0;
}
