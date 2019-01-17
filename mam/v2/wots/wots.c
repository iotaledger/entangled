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
\file wots.c
\brief MAM2 WOTS layer.
*/

#include <stdlib.h>
#include <string.h>

#include "mam/v2/wots/wots.h"

static void wots_calc_pks(spongos_t *s, trits_t sk_pks, trits_t pk) {
  size_t i, j;
  trits_t sk_part, pks = sk_pks;

  MAM2_ASSERT(trits_size(sk_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sk_part = trits_take(sk_pks, MAM2_WOTS_SK_PART_SIZE);
    sk_pks = trits_drop(sk_pks, MAM2_WOTS_SK_PART_SIZE);

    for (j = 0; j < 26; ++j) spongos_hash(s, sk_part, sk_part);
  }

  spongos_hash(s, pks, pk);
}

static void wots_hash_sign(spongos_t *s, trits_t sk_sig, trits_t H) {
  size_t i;
  trint9_t t = 0;
  trint3_t j, h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sk_sig) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(H) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(H, i * 3));
    t += h;

    for (j = -13; j < h; ++j) spongos_hash(s, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = -13; j < h; ++j) spongos_hash(s, sig_part, sig_part);
  }
}

static void wots_hash_verify(spongos_t *s, trits_t sig_pks, trits_t H) {
  size_t i;
  trint9_t t = 0;
  trint3_t j, h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sig_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(H) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(H, i * 3));
    t += h;

    for (j = -13; j < -h; ++j) spongos_hash(s, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = -13; j < -h; ++j) spongos_hash(s, sig_part, sig_part);
  }
}

trits_t wots_sk_trits(wots_t *w) {
  return trits_from_rep(MAM2_WOTS_SK_SIZE, w->sk);
}

void wots_init(wots_t *w, sponge_t *s) {
  MAM2_ASSERT(w);
  MAM2_ASSERT(s);
  w->sg->sponge = s;
}

void wots_gen_sk(wots_t *w, prng_t *p, trits_t N) {
  wots_gen_sk3(w, p, N, trits_null(), trits_null());
}

void wots_gen_sk2(wots_t *w, prng_t *p, trits_t N1, trits_t N2) {
  wots_gen_sk3(w, p, N1, N2, trits_null());
}

void wots_gen_sk3(wots_t *w, prng_t *p, trits_t N1, trits_t N2, trits_t N3) {
  prng_gen3(p, MAM2_PRNG_DST_WOTSKEY, N1, N2, N3, wots_sk_trits(w));
}

void wots_calc_pk(wots_t *w, trits_t pk) {
  MAM2_TRITS_DEF0(sk_pks, MAM2_WOTS_SK_SIZE);
  sk_pks = MAM2_TRITS_INIT(sk_pks, MAM2_WOTS_SK_SIZE);

  trits_copy(wots_sk_trits(w), sk_pks);
  wots_calc_pks(w->sg, sk_pks, pk);
  trits_set_zero(sk_pks);
}

void wots_sign(wots_t *w, trits_t H, trits_t sig) {
  trits_copy(wots_sk_trits(w), sig);
  wots_hash_sign(w->sg, sig, H);
}

void wots_recover(spongos_t *s, trits_t H, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF0(sig_pks, MAM2_WOTS_SK_SIZE);
  sig_pks = MAM2_TRITS_INIT(sig_pks, MAM2_WOTS_SK_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  trits_copy(sig, sig_pks);
  wots_hash_verify(s, sig_pks, H);
  spongos_hash(s, sig_pks, pk);
}

bool wots_verify(spongos_t *s, trits_t H, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF0(sig_pk, MAM2_WOTS_PK_SIZE);
  sig_pk = MAM2_TRITS_INIT(sig_pk, MAM2_WOTS_PK_SIZE);

  wots_recover(s, H, sig, sig_pk);
  return trits_cmp_eq(pk, sig_pk);
}

retcode_t wots_create(wots_t *w) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  MAM2_ASSERT(w);
  do {
    memset(w, 0, sizeof(wots_t));
    w->sk = malloc(sizeof(word_t) * MAM2_WORDS(MAM2_WOTS_SK_SIZE));
    err_guard(w->sk, RC_OOM);
    e = RC_OK;
  } while (0);
  return e;
}

void wots_destroy(wots_t *w) {
  MAM2_ASSERT(w);
  free(w->sk);
  w->sk = 0;
  free(w);
}
