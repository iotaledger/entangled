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
#include <string.h>

#include "mam/v2/pb3/pb3.h"
#include "mam/v2/sponge/spongos_types.h"
#include "mam/v2/wots/wots.h"

/*
 * Private functions
 */

static void mam_wots_calc_pks(mam_spongos_t *const spongos, trits_t secret_key,
                              trits_t public_key) {
  trits_t secret_key_part;

  for (size_t i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    secret_key_part = trits_take(secret_key, MAM2_WOTS_SK_PART_SIZE);
    secret_key = trits_drop(secret_key, MAM2_WOTS_SK_PART_SIZE);

    for (size_t j = 0; j < 26; ++j) {
      mam_spongos_hash(spongos, secret_key_part, secret_key_part);
    }
  }
}

typedef enum wots_hash_operation_e {
  WOTS_HASH_SIGN,
  WOTS_HASH_RECOVER
} wots_hash_operation_t;

static void wots_hash_sign_or_recover(mam_spongos_t *const spongos,
                                      trits_t signature, trits_t const hash,
                                      wots_hash_operation_t const operation) {
  size_t i;
  trint9_t t = 0;
  trint3_t j, h;
  trits_t signature_part;

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    signature_part = trits_take(signature, MAM2_WOTS_SK_PART_SIZE);
    signature = trits_drop(signature, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(hash, i * 3));
    t += h;
    h = (operation == WOTS_HASH_SIGN ? h : -h);

    for (j = -13; j < h; ++j) {
      mam_spongos_hash(spongos, signature_part, signature_part);
    }
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    signature_part = trits_take(signature, MAM2_WOTS_SK_PART_SIZE);
    signature = trits_drop(signature, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);
    h = (operation == WOTS_HASH_SIGN ? h : -h);

    for (j = -13; j < h; ++j) {
      mam_spongos_hash(spongos, signature_part, signature_part);
    }
  }
}

static inline trits_t wots_secret_key_trits(mam_wots_t const *const wots) {
  return trits_from_rep(MAM2_WOTS_SK_SIZE, wots->secret_key);
}

/*
 * Public functions
 */

void mam_wots_init(mam_wots_t *const wots) {
  MAM2_ASSERT(wots);
  memset(wots->secret_key, 0, MAM2_WOTS_SK_SIZE);
}

void mam_wots_destroy(mam_wots_t *const wots) {
  MAM2_ASSERT(wots);

  memset(wots, 0, sizeof(mam_wots_t));
}

void mam_wots_gen_sk(mam_wots_t *const wots, mam_prng_t const *const prng,
                     trits_t const nonce) {
  mam_wots_gen_sk3(wots, prng, nonce, trits_null(), trits_null());
}

void mam_wots_gen_sk2(mam_wots_t *const wots, mam_prng_t const *const prng,
                      trits_t const nonce1, trits_t const nonce2) {
  mam_wots_gen_sk3(wots, prng, nonce1, nonce2, trits_null());
}

void mam_wots_gen_sk3(mam_wots_t *const wots, mam_prng_t const *const prng,
                      trits_t const nonce1, trits_t const nonce2,
                      trits_t const nonce3) {
  mam_prng_gen3(prng, MAM2_PRNG_DST_WOTS_KEY, nonce1, nonce2, nonce3,
                wots_secret_key_trits(wots));
}

void mam_wots_calc_pk(mam_wots_t *const wots, trits_t public_key) {
  mam_spongos_t spongos;
  mam_spongos_init(&spongos);
  MAM2_ASSERT(trits_size(public_key) == MAM2_WOTS_PK_SIZE);

  MAM2_TRITS_DEF0(secret_key, MAM2_WOTS_SK_SIZE);
  secret_key = MAM2_TRITS_INIT(secret_key, MAM2_WOTS_SK_SIZE);

  trits_copy(wots_secret_key_trits(wots), secret_key);
  mam_wots_calc_pks(&spongos, secret_key, public_key);
  mam_spongos_hash(&spongos, secret_key, public_key);

  trits_set_zero(secret_key);
}

void mam_wots_sign(mam_wots_t *const wots, trits_t const hash,
                   trits_t signature) {
  mam_spongos_t spongos;
  mam_spongos_init(&spongos);
  MAM2_ASSERT(trits_size(hash) == MAM2_WOTS_HASH_SIZE);
  MAM2_ASSERT(trits_size(signature) == MAM2_WOTS_SK_SIZE);

  trits_copy(wots_secret_key_trits(wots), signature);
  wots_hash_sign_or_recover(&spongos, signature, hash, WOTS_HASH_SIGN);
}

void mam_wots_recover(mam_spongos_t *const spongos, trits_t const hash,
                      trits_t const signature, trits_t public_key) {
  MAM2_ASSERT(trits_size(hash) == MAM2_WOTS_HASH_SIZE);
  MAM2_ASSERT(trits_size(signature) == MAM2_WOTS_SK_SIZE);

  MAM2_TRITS_DEF0(sig_pks, MAM2_WOTS_SK_SIZE);
  sig_pks = MAM2_TRITS_INIT(sig_pks, MAM2_WOTS_SK_SIZE);

  trits_copy(signature, sig_pks);
  wots_hash_sign_or_recover(spongos, sig_pks, hash, WOTS_HASH_RECOVER);
  mam_spongos_hash(spongos, sig_pks, public_key);
}

bool mam_wots_verify(mam_spongos_t *const spongos, trits_t const hash,
                     trits_t const signature, trits_t const public_key) {
  MAM2_TRITS_DEF0(recovered_public_key, MAM2_WOTS_PK_SIZE);
  recovered_public_key =
      MAM2_TRITS_INIT(recovered_public_key, MAM2_WOTS_PK_SIZE);

  mam_wots_recover(spongos, hash, signature, recovered_public_key);
  return trits_cmp_eq(public_key, recovered_public_key);
}
