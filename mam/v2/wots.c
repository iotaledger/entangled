/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/wots.h"

/*
 * Private functions
 */

static void wots_calc_pks(isponge *sponge, trits_t sk_pks, trits_t pk) {
  size_t i, j;
  trits_t sk_part, pks = sk_pks;

  MAM2_ASSERT(trits_size(sk_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sk_part = trits_take(sk_pks, MAM2_WOTS_SK_PART_SIZE);
    sk_pks = trits_drop(sk_pks, MAM2_WOTS_SK_PART_SIZE);

    for (j = 0; j < 26; ++j) sponge_hash(sponge, sk_part, sk_part);
  }

  sponge_hash(sponge, pks, pk);
}

static void wots_hash_sign(isponge *sponge, trits_t sk_sig, trits_t hash) {
  size_t i, j;
  trint9_t t = 0;
  trint3_t h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sk_sig) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(hash) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(hash, i * 3));
    t += h;

    for (j = 0; j < 13 + h; ++j) sponge_hash(sponge, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 + h; ++j) sponge_hash(sponge, sig_part, sig_part);
  }
}

static void wots_hash_verify(isponge *sponge, trits_t sig_pks, trits_t hash) {
  size_t i, j;
  trint9_t t = 0;
  trint3_t h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sig_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(hash) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(hash, i * 3));
    t += h;

    for (j = 0; j < 13 - h; ++j) sponge_hash(sponge, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 - h; ++j) sponge_hash(sponge, sig_part, sig_part);
  }
}

/*
 * Public functions
 */

void wots_init(wots_t *wots, isponge *sponge) {
  MAM2_ASSERT(wots);
  MAM2_ASSERT(sponge);
  wots->sponge = sponge;
}

void wots_gen_sk(wots_t *wots, prng_t *prng, trits_t nonce) {
  wots_gen_sk3(wots, prng, nonce, trits_null(), trits_null());
}

void wots_gen_sk2(wots_t *wots, prng_t *prng, trits_t nonce1, trits_t nonce2) {
  wots_gen_sk3(wots, prng, nonce1, nonce2, trits_null());
}

void wots_gen_sk3(wots_t *wots, prng_t *prng, trits_t nonce1, trits_t nonce2,
                  trits_t nonce3) {
  prng_gen3(prng, MAM2_PRNG_DST_WOTS_KEY, nonce1, nonce2, nonce3,
            wots_sk_trits(wots));
}

void wots_calc_pk(wots_t *wots, trits_t pk) {
  MAM2_TRITS_DEF(sk_pks, MAM2_WOTS_SK_SIZE);
  trits_copy(wots_sk_trits(wots), sk_pks);
  size_t num_sk_pks = wots_sk_trits(wots).n - wots_sk_trits(wots).d;
  TRIT_ARRAY_MAKE_FROM_RAW(sk_pks_arr, num_sk_pks, wots_sk_trits(wots).p);
  wots_calc_pks(wots->sponge, sk_pks, pk);
  trits_set_zero(sk_pks);
}

void wots_sign(wots_t *wots, trits_t hash, trits_t sig) {
  trits_copy(wots_sk_trits(wots), sig);
  wots_hash_sign(wots->sponge, sig, hash);
}

void wots_recover(isponge *sponge, trits_t hash, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF(sig_pks, MAM2_WOTS_SK_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  trits_copy(sig, sig_pks);
  wots_hash_verify(sponge, sig_pks, hash);
  sponge_hash(sponge, sig_pks, pk);
}

bool_t wots_verify(isponge *sponge, trits_t hash, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF(sig_pk, MAM2_WOTS_PK_SIZE);
  wots_recover(sponge, hash, sig, sig_pk);
  return (0 == trits_cmp_grlex(pk, sig_pk)) ? 1 : 0;
}

err_t wots_create(wots_t *wots) {
  err_t e = err_internal_error;
  MAM2_ASSERT(wots);
  do {
    memset(wots, 0, sizeof(wots_t));
    wots->sk = (trit_t *)malloc(sizeof(trit_t) * MAM2_WORDS(MAM2_WOTS_SK_SIZE));
    memset(wots->sk, 0, MAM2_WORDS(MAM2_WOTS_SK_SIZE));
    err_guard(wots->sk, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

void wots_destroy(wots_t *wots) {
  MAM2_ASSERT(wots);
  free(wots->sk);
  wots->sk = NULL;
}

trits_t wots_sk_trits(wots_t *wots) {
  return trits_from_rep(MAM2_WOTS_SK_SIZE, wots->sk);
}
