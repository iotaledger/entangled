/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/trinary/trit_long.h"
#include "mam/v2/wots.h"

/*
 * Private functions
 */

static void wots_calc_pks(sponge_t *const sponge, trit_array_t *const sk_pks,
                          trit_array_t *const pk) {
  size_t i, j;

  TRIT_ARRAY_DECLARE(sk_part_trits_array, MAM2_WOTS_SK_PART_SIZE);
  MAM2_ASSERT(sk_pks->num_trits == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(pk->num_trits == MAM2_WOTS_PK_SIZE);

  size_t sk_pos = 0;
  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    trit_array_insert_from_pos(&sk_part_trits_array, sk_pks, sk_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);
    for (j = 0; j < 26; ++j) {
      sponge_hash(sponge, &sk_part_trits_array, &sk_part_trits_array);
    }
    trit_array_insert_from_pos(sk_pks, &sk_part_trits_array, 0, sk_pos,
                               MAM2_WOTS_SK_PART_SIZE);
    sk_pos += MAM2_WOTS_SK_PART_SIZE;
  }
  sponge_hash(sponge, sk_pks, pk);
}

static void wots_hash_sign(sponge_t *const sponge,
                           trit_array_t const *const hash,
                           trit_array_t *const sk_sig) {
  size_t i, j;
  trint9_t t = 0;
  trint3_t h;
  flex_trit_t tmp_flex[NUM_FLEX_TRITS_FOR_TRITS(3)];
  trit_t h_trits[3];
  size_t sig_pos = 0;

  MAM2_ASSERT(hash->num_trits == MAM2_WOTS_HASH_SIZE);

  TRIT_ARRAY_DECLARE(sk_sig_part, MAM2_WOTS_SK_PART_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    trit_array_insert_from_pos(&sk_sig_part, sk_sig, sig_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);
    flex_trits_slice(tmp_flex, 3, hash->trits, MAM2_WOTS_HASH_SIZE, i * 3, 3);
    flex_trits_to_trits(h_trits, 3, tmp_flex, 3, 3);
    h = trits_to_long(h_trits, 3);
    t += h;

    for (j = 0; j < 13 + h; ++j) {
      sponge_hash(sponge, &sk_sig_part, &sk_sig_part);
    }
    trit_array_insert_from_pos(sk_sig, &sk_sig_part, 0, sig_pos,
                               MAM2_WOTS_SK_PART_SIZE);
    sig_pos += MAM2_WOTS_SK_PART_SIZE;
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    trit_array_insert_from_pos(&sk_sig_part, sk_sig, sig_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);
    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 + h; ++j) {
      sponge_hash(sponge, &sk_sig_part, &sk_sig_part);
    }
    trit_array_insert_from_pos(sk_sig, &sk_sig_part, 0, sig_pos,
                               MAM2_WOTS_SK_PART_SIZE);
    sig_pos += MAM2_WOTS_SK_PART_SIZE;
  }
}

static void wots_hash_verify(sponge_t *const sponge,
                             trit_array_t const *const hash,
                             trit_array_t *const sig_pks) {
  size_t i, j;
  trint9_t t = 0;
  trint3_t h;
  flex_trit_t tmp_flex[NUM_FLEX_TRITS_FOR_TRITS(3)];
  trit_t h_trits[3];
  size_t sig_pos = 0;

  MAM2_ASSERT(sig_pks->num_trits == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(hash->num_trits == MAM2_WOTS_HASH_SIZE);
  TRIT_ARRAY_DECLARE(sk_sig_part, MAM2_WOTS_SK_PART_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    trit_array_insert_from_pos(&sk_sig_part, sig_pks, sig_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);
    flex_trits_slice(tmp_flex, 3, hash->trits, MAM2_WOTS_HASH_SIZE, i * 3, 3);
    flex_trits_to_trits(h_trits, 3, tmp_flex, 3, 3);
    h = trits_to_long(h_trits, 3);
    t += h;

    for (j = 0; j < 13 - h; ++j) {
      sponge_hash(sponge, &sk_sig_part, &sk_sig_part);
    }
    trit_array_insert_from_pos(sig_pks, &sk_sig_part, 0, sig_pos,
                               MAM2_WOTS_SK_PART_SIZE);
    sig_pos += MAM2_WOTS_SK_PART_SIZE;
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    trit_array_insert_from_pos(&sk_sig_part, sig_pks, sig_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 - h; ++j) {
      sponge_hash(sponge, &sk_sig_part, &sk_sig_part);
    };

    trit_array_insert_from_pos(sig_pks, &sk_sig_part, 0, sig_pos,
                               MAM2_WOTS_SK_PART_SIZE);
    sig_pos += MAM2_WOTS_SK_PART_SIZE;
  }
}

/*
 * Public functions
 */

void wots_init(wots_t *const wots, sponge_t *const sponge) {
  MAM2_ASSERT(wots);
  MAM2_ASSERT(sponge);
  wots->sponge = sponge;
}

void wots_reset(wots_t *const wots) {
  MAM2_ASSERT(wots);
  wots->sponge = NULL;
  memset(wots->sk, FLEX_TRIT_NULL_VALUE, MAM2_WOTS_SK_FLEX_SIZE);
}

void wots_gen_sk(wots_t *const wots, prng_t *const prng,
                 trit_array_t const *const nonce) {
  trit_array_t null_trits = trit_array_null();
  wots_gen_sk3(wots, prng, nonce, &null_trits, &null_trits);
}

void wots_gen_sk2(wots_t *const wots, prng_t *const prng,
                  trit_array_t const *const nonce1,
                  trit_array_t const *const nonce2) {
  trit_array_t null_trits = trit_array_null();
  wots_gen_sk3(wots, prng, nonce1, nonce2, &null_trits);
}

void wots_gen_sk3(wots_t *const wots, prng_t *const prng,
                  trit_array_t const *const nonce1,
                  trit_array_t const *const nonce2,
                  trit_array_t const *const nonce3) {
  trit_array_t sk = {.trits = wots->sk,
                     .num_trits = MAM2_WOTS_SK_SIZE,
                     .num_bytes = MAM2_WOTS_SK_FLEX_SIZE,
                     .dynamic = 0};
  prng_gen3(prng, MAM2_PRNG_DST_WOTS_KEY, nonce1, nonce2, nonce3, &sk);
}

void wots_calc_pk(wots_t *const wots, trit_array_t *const pk) {
  TRIT_ARRAY_DECLARE(sk_trits_array, MAM2_WOTS_SK_SIZE);
  memcpy(sk_trits_array.trits, wots->sk, MAM2_WOTS_SK_FLEX_SIZE);
  wots_calc_pks(wots->sponge, &sk_trits_array, pk);
}

void wots_sign(wots_t *const wots, trit_array_t const *const hash,
               trit_array_t *const sig) {
  wots_hash_sign(wots->sponge, hash, sig);
}

void wots_recover(sponge_t *const sponge, trit_array_t const *const hash,
                  trit_array_t *const sig, trit_array_t *const pk) {
  MAM2_ASSERT(pk->num_trits == MAM2_WOTS_PK_SIZE);
  wots_hash_verify(sponge, hash, sig);
  sponge_hash(sponge, sig, pk);
}

bool wots_verify(sponge_t *const sponge, trit_array_t const *const hash,
                 trit_array_t *const sig, trit_array_t const *const pk) {
  TRIT_ARRAY_DECLARE(pk_array, MAM2_WOTS_PK_SIZE);
  wots_recover(sponge, hash, sig, &pk_array);
  return (0 == memcmp(pk->trits, pk_array.trits,
                      trit_array_bytes_for_trits(MAM2_WOTS_PK_SIZE)));
}
