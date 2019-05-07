/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/wots/wots.h"

/*
 * Private functions
 */

typedef enum wots_hash_operation_e { WOTS_HASH_SIGN, WOTS_HASH_RECOVER } wots_hash_operation_t;

static void wots_hash_sign_or_recover(mam_spongos_t *const spongos, trits_t const hash, trits_t signature,
                                      wots_hash_operation_t const operation) {
  size_t i;
  trint9_t t = 0;
  trint3_t j, h;
  trits_t signature_part;

  for (i = 0; i < MAM_WOTS_PRIVATE_KEY_PART_COUNT - 3; ++i) {
    signature_part = trits_take(signature, MAM_WOTS_PRIVATE_KEY_PART_SIZE);
    signature = trits_drop(signature, MAM_WOTS_PRIVATE_KEY_PART_SIZE);

    h = trits_get3(trits_drop(hash, i * 3));
    t += h;
    h = (operation == WOTS_HASH_SIGN ? h : -h);

    for (j = -13; j < h; ++j) {
      mam_spongos_hash(spongos, signature_part, signature_part);
    }
  }

  t = -t;
  for (; i < MAM_WOTS_PRIVATE_KEY_PART_COUNT; ++i) {
    signature_part = trits_take(signature, MAM_WOTS_PRIVATE_KEY_PART_SIZE);
    signature = trits_drop(signature, MAM_WOTS_PRIVATE_KEY_PART_SIZE);

    h = MAM_MODS(t, 19683, 27);
    t = MAM_DIVS(t, 19683, 27);
    h = (operation == WOTS_HASH_SIGN ? h : -h);

    for (j = -13; j < h; ++j) {
      mam_spongos_hash(spongos, signature_part, signature_part);
    }
  }
}

/*
 * Public functions
 */

retcode_t mam_wots_gen_pk(mam_wots_t const *const wots, trits_t public_key) {
  mam_spongos_t spongos;
  mam_spongos_t spongos_part;
  trit_t private_key_part[MAM_WOTS_PRIVATE_KEY_PART_SIZE];

  if (wots == NULL) {
    return RC_NULL_PARAM;
  }

  if (trits_size(public_key) != MAM_WOTS_PUBLIC_KEY_SIZE) {
    return RC_INVALID_PARAM;
  }

  mam_spongos_init(&spongos_part);
  for (size_t i = 0; i < MAM_WOTS_PRIVATE_KEY_PART_COUNT; ++i) {
    memcpy(private_key_part, wots->private_key + MAM_WOTS_PRIVATE_KEY_PART_SIZE * i, MAM_WOTS_PRIVATE_KEY_PART_SIZE);
    for (size_t j = 0; j < 26; ++j) {
      mam_spongos_hash(&spongos, trits_from_rep(MAM_WOTS_PRIVATE_KEY_PART_SIZE, private_key_part),
                       trits_from_rep(MAM_WOTS_PRIVATE_KEY_PART_SIZE, private_key_part));
    }
    mam_spongos_absorb(&spongos_part, trits_from_rep(MAM_WOTS_PRIVATE_KEY_PART_SIZE, private_key_part));
  }
  mam_spongos_commit(&spongos_part);
  mam_spongos_squeeze(&spongos_part, public_key);

  return RC_OK;
}

retcode_t mam_wots_sign(mam_wots_t const *const wots, trits_t const hash, trits_t signature) {
  mam_spongos_t spongos;

  if (wots == NULL) {
    return RC_NULL_PARAM;
  }

  if (trits_size(hash) != MAM_WOTS_HASH_SIZE || trits_size(signature) != MAM_WOTS_SIGNATURE_SIZE) {
    return RC_INVALID_PARAM;
  }

  memcpy(trits_begin(signature), wots->private_key, MAM_WOTS_PRIVATE_KEY_SIZE);
  wots_hash_sign_or_recover(&spongos, hash, signature, WOTS_HASH_SIGN);

  return RC_OK;
}

retcode_t mam_wots_recover(trits_t const hash, trits_t const signature, trits_t public_key) {
  mam_spongos_t spongos;
  trit_t sig_pks[MAM_WOTS_PRIVATE_KEY_SIZE];

  if (trits_size(hash) != MAM_WOTS_HASH_SIZE || trits_size(signature) != MAM_WOTS_SIGNATURE_SIZE ||
      trits_size(public_key) != MAM_WOTS_PUBLIC_KEY_SIZE) {
    return RC_INVALID_PARAM;
  }

  memcpy(sig_pks, trits_begin(signature), MAM_WOTS_SIGNATURE_SIZE);
  wots_hash_sign_or_recover(&spongos, hash, trits_from_rep(MAM_WOTS_PRIVATE_KEY_SIZE, sig_pks), WOTS_HASH_RECOVER);
  mam_spongos_hash(&spongos, trits_from_rep(MAM_WOTS_PRIVATE_KEY_SIZE, sig_pks), public_key);

  return RC_OK;
}
