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

static void wots_calc_pks(isponge *const sponge, trits_t sk_pks, trits_t pk) {
  size_t i, j;
  trits_t sk_part, pks = sk_pks;

  MAM2_ASSERT(trits_size(sk_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sk_part = trits_take(sk_pks, MAM2_WOTS_SK_PART_SIZE);
    sk_pks = trits_drop(sk_pks, MAM2_WOTS_SK_PART_SIZE);

    for (j = 0; j < 26; ++j) {
      sponge_hash(sponge, sk_part, sk_part);
    }
  }

  sponge_hash(sponge, pks, pk);
}

static void wots_hash_sign(isponge *const sponge, trits_t sk_sig,
                           trits_t const hash) {
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

    for (j = 0; j < 13 + h; ++j) {
      sponge_hash(sponge, sig_part, sig_part);
    }
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 + h; ++j) {
      sponge_hash(sponge, sig_part, sig_part);
    }
  }
}

static void wots_hash_verify(isponge *const sponge, trits_t sig_pks,
                             trits_t const hash) {
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

    for (j = 0; j < 13 - h; ++j) {
      sponge_hash(sponge, sig_part, sig_part);
    };
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 - h; ++j) {
      sponge_hash(sponge, sig_part, sig_part);
    };
  }
}

/*
 * Public functions
 */

void wots_init(wots_t *const wots, isponge *const sponge) {
  MAM2_ASSERT(wots);
  MAM2_ASSERT(sponge);
  wots->sponge = sponge;
}

void wots_reset(wots_t *const wots) {
  MAM2_ASSERT(wots);
  wots->sponge = NULL;
  memset(wots->sk, FLEX_TRIT_NULL_VALUE, MAM2_WOTS_SK_FLEX_SIZE);
}

void wots_gen_sk(wots_t *const wots, prng_t *const prng, trits_t const nonce) {
  wots_gen_sk3(wots, prng, nonce, trits_null(), trits_null());
}

void wots_gen_sk2(wots_t *const wots, prng_t *const prng, trits_t const nonce1,
                  trits_t const nonce2) {
  wots_gen_sk3(wots, prng, nonce1, nonce2, trits_null());
}

void wots_gen_sk3(wots_t *const wots, prng_t *const prng, trits_t const nonce1,
                  trits_t const nonce2, trits_t const nonce3) {
  // TODO Remove when prng_gen3 takes flex_trit_t *
  MAM2_TRITS_DEF(tmp, MAM2_WOTS_SK_SIZE);
  prng_gen3(prng, MAM2_PRNG_DST_WOTS_KEY, nonce1, nonce2, nonce3, tmp);
  // TODO Remove when prng_gen3 takes flex_trit_t *
  flex_trits_from_trits(wots->sk, MAM2_WOTS_SK_SIZE, tmp.p + tmp.d,
                        MAM2_WOTS_SK_SIZE, MAM2_WOTS_SK_SIZE);
}

void wots_calc_pk(wots_t *const wots, trits_t pk) {
  MAM2_TRITS_DEF(sk_pks, MAM2_WOTS_SK_SIZE);
  // TODO Remove when wots_calc_pks takes flex_trit_t *
  flex_trits_to_trits(sk_pks.p + sk_pks.d, MAM2_WOTS_SK_SIZE, wots->sk,
                      MAM2_WOTS_SK_SIZE, MAM2_WOTS_SK_SIZE);
  wots_calc_pks(wots->sponge, sk_pks, pk);
  trits_set_zero(sk_pks);
}

void wots_sign(wots_t *const wots, trits_t const hash, trits_t sig) {
  // TODO Remove when wots_hash_sign takes flex_trit_t *
  flex_trits_to_trits(sig.p + sig.d, MAM2_WOTS_SK_SIZE, wots->sk,
                      MAM2_WOTS_SK_SIZE, MAM2_WOTS_SK_SIZE);
  wots_hash_sign(wots->sponge, sig, hash);
}

void wots_recover(isponge *const sponge, trits_t const hash, trits_t const sig,
                  trits_t pk) {
  MAM2_TRITS_DEF(sig_pks, MAM2_WOTS_SK_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  trits_copy(sig, sig_pks);
  wots_hash_verify(sponge, sig_pks, hash);
  sponge_hash(sponge, sig_pks, pk);
}

bool wots_verify(isponge *const sponge, trits_t const hash, trits_t const sig,
                 trits_t const pk) {
  MAM2_TRITS_DEF(sig_pk, MAM2_WOTS_PK_SIZE);
  wots_recover(sponge, hash, sig, sig_pk);
  return (0 == trits_cmp_grlex(pk, sig_pk));
}
