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

static void wots_calc_pks(sponge_t *const sponge, trit_array_t *const sk,
                          trit_array_t *const pk) {
  TRIT_ARRAY_DECLARE(sk_part, MAM2_WOTS_SK_PART_SIZE);

  size_t sk_pos = 0;
  for (size_t i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    trit_array_insert_from_pos(&sk_part, sk, sk_pos, 0, MAM2_WOTS_SK_PART_SIZE);
    for (size_t j = 0; j < 26; ++j) {
      sponge_hash(sponge, &sk_part, &sk_part);
    }
    trit_array_insert_from_pos(sk, &sk_part, 0, sk_pos, MAM2_WOTS_SK_PART_SIZE);
    sk_pos += MAM2_WOTS_SK_PART_SIZE;
  }
  sponge_hash(sponge, sk, pk);
}

typedef enum wots_hash_operation_e {
  WOTS_HASH_SIGN,
  WOTS_HASH_RECOVER
} wots_hash_operation_t;

static void wots_hash_sign_or_recover(sponge_t *const sponge,
                                      trit_array_t const *const hash,
                                      trit_array_t *const sig,
                                      wots_hash_operation_t const operation) {
  size_t i, j;
  int16_t t = 0;
  int8_t h = 0;
  flex_trit_t hash_value_flex[NUM_FLEX_TRITS_FOR_TRITS(3)];
  trit_t hash_value_trits[3];
  size_t sig_pos = 0;
  TRIT_ARRAY_DECLARE(sig_part, MAM2_WOTS_SK_PART_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    trit_array_insert_from_pos(&sig_part, sig, sig_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);
    flex_trits_slice(hash_value_flex, 3, hash->trits, MAM2_WOTS_HASH_SIZE,
                     i * 3, 3);
    flex_trits_to_trits(hash_value_trits, 3, hash_value_flex, 3, 3);
    h = trits_to_long(hash_value_trits, 3);
    t += h;

    h = (operation == WOTS_HASH_SIGN) ? h : -h;
    for (j = 0; j < 13 + h; ++j) {
      sponge_hash(sponge, &sig_part, &sig_part);
    }
    trit_array_insert_from_pos(sig, &sig_part, 0, sig_pos,
                               MAM2_WOTS_SK_PART_SIZE);
    sig_pos += MAM2_WOTS_SK_PART_SIZE;
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    trit_array_insert_from_pos(&sig_part, sig, sig_pos, 0,
                               MAM2_WOTS_SK_PART_SIZE);
    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    h = (operation == WOTS_HASH_SIGN) ? h : -h;
    for (j = 0; j < 13 + h; ++j) {
      sponge_hash(sponge, &sig_part, &sig_part);
    };

    trit_array_insert_from_pos(sig, &sig_part, 0, sig_pos,
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
  MAM2_ASSERT(pk->num_trits == MAM2_WOTS_PK_SIZE);
  TRIT_ARRAY_DECLARE(sk, MAM2_WOTS_SK_SIZE);
  memcpy(sk.trits, wots->sk, MAM2_WOTS_SK_FLEX_SIZE);
  wots_calc_pks(wots->sponge, &sk, pk);
}

void wots_sign(wots_t *const wots, trit_array_t const *const hash,
               trit_array_t *const sig) {
  MAM2_ASSERT(hash->num_trits == MAM2_WOTS_HASH_SIZE);
  MAM2_ASSERT(sig->num_trits == MAM2_WOTS_SIG_SIZE);
  wots_hash_sign_or_recover(wots->sponge, hash, sig, WOTS_HASH_SIGN);
}

void wots_recover(sponge_t *const sponge, trit_array_t const *const hash,
                  trit_array_t const *const sig, trit_array_t *const pk) {
  MAM2_ASSERT(hash->num_trits == MAM2_WOTS_HASH_SIZE);
  MAM2_ASSERT(sig->num_trits == MAM2_WOTS_SIG_SIZE);
  MAM2_ASSERT(pk->num_trits == MAM2_WOTS_PK_SIZE);
  TRIT_ARRAY_DECLARE(sig_cpy, MAM2_WOTS_SIG_SIZE);
  memcpy(sig_cpy.trits, sig->trits, MAM2_WOTS_SIG_FLEX_SIZE);
  wots_hash_sign_or_recover(sponge, hash, &sig_cpy, WOTS_HASH_RECOVER);
  sponge_hash(sponge, &sig_cpy, pk);
}

bool wots_verify(sponge_t *const sponge, trit_array_t const *const hash,
                 trit_array_t const *const sig, trit_array_t const *const pk) {
  MAM2_ASSERT(hash->num_trits == MAM2_WOTS_HASH_SIZE);
  MAM2_ASSERT(sig->num_trits == MAM2_WOTS_SIG_SIZE);
  MAM2_ASSERT(pk->num_trits == MAM2_WOTS_PK_SIZE);
  TRIT_ARRAY_DECLARE(pk_recovered, MAM2_WOTS_PK_SIZE);
  wots_recover(sponge, hash, sig, &pk_recovered);
  return (0 == memcmp(pk->trits, pk_recovered.trits, MAM2_WOTS_PK_FLEX_SIZE));
}
