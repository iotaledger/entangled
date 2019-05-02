/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __MAM_WOTS_WOTS_H__
#define __MAM_WOTS_WOTS_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/prng/prng.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"

/** @brief WOTS public key size */
#define MAM_WOTS_PK_SIZE 243
/** @brief WOTS secret key part size */
#define MAM_WOTS_SK_PART_SIZE 162
/** @brief WOTS secret key parts count */
#define MAM_WOTS_SK_PART_COUNT 81
/** @brief WOTS secret key size */
#define MAM_WOTS_SK_SIZE (MAM_WOTS_SK_PART_SIZE * MAM_WOTS_SK_PART_COUNT)
/** @brief WOTS signed hash value size */
#define MAM_WOTS_HASH_SIZE 234
/** @brief WOTS signature size */
#define MAM_WOTS_SIG_SIZE MAM_WOTS_SK_SIZE

#ifdef __cplusplus
extern "C" {
#endif

// WOTS layer interface
typedef struct mam_wots_s {
  trit_t secret_key[MAM_WOTS_SK_SIZE];
} mam_wots_t;

/**
 * @brief Safely resets a WOTS secret key
 *
 * @param[out] wots The WOTS secret key
 */
void mam_wots_reset(mam_wots_t *const wots);

// TODO
static inline trits_t wots_secret_key_trits(mam_wots_t const *const wots) {
  return trits_from_rep(MAM_WOTS_SK_SIZE, wots->secret_key);
}

/**
 * @brief Generates a WOTS secret key with three nonces
 *
 * @param[out] wots The WOTS secret key
 * @param[in] prng A PRNG
 * @param[in] nonce1 The first nonce
 * @param[in] nonce2 The second nonce
 * @param[in] nonce3 The third nonce
 */
static inline void mam_wots_gen_sk3(mam_wots_t *const wots, mam_prng_t const *const prng, trits_t const nonce1,
                                    trits_t const nonce2, trits_t const nonce3) {
  mam_prng_gen3(prng, MAM_PRNG_DST_WOTS_KEY, nonce1, nonce2, nonce3, wots_secret_key_trits(wots));
}

/**
 * @brief Generates a WOTS secret key with two nonces
 *
 * @param[out] wots The WOTS secret key
 * @param[in] prng A PRNG
 * @param[in] nonce1 The first nonce
 * @param[in] nonce2 The second nonce
 */
static inline void mam_wots_gen_sk2(mam_wots_t *const wots, mam_prng_t const *const prng, trits_t const nonce1,
                                    trits_t const nonce2) {
  mam_wots_gen_sk3(wots, prng, nonce1, nonce2, trits_null());
}

/**
 * @brief Generates a WOTS secret key with a nonce
 *
 * @param[out] wots The WOTS secret key
 * @param[in] prng A PRNG
 * @param[in] nonce The nonce
 */
static inline void mam_wots_gen_sk(mam_wots_t *const wots, mam_prng_t const *const prng, trits_t const nonce) {
  mam_wots_gen_sk3(wots, prng, nonce, trits_null(), trits_null());
}

/**
 * @brief Generates a WOTS public key associated with a WOTS private key
 *
 * The private key must have already been generated
 *
 * @param[in] wots The WOTS secret key
 * @param[out] public_key The WOTS public key
 */
void mam_wots_calc_pk(mam_wots_t const *const wots, trits_t public_key);

/**
 * @brief Generates a WOTS signature associated with a WOTS private key
 *
 * @param[in] wots The WOTS secret key
 * @param[in] hash A hash to be signed
 * @param[out] signature The WOTS signature
 */
void mam_wots_sign(mam_wots_t const *const wots, trits_t const hash, trits_t signature);

/**
 * @brief Recovers a WOTS public key from a WOTS signature
 *
 * @param[in] hash A signed hash
 * @param[in] signature The WOTS signature
 * @param[out] public_key The WOTS public key
 */
void mam_wots_recover(trits_t const hash, trits_t const signature, trits_t public_key);

/**
 * @brief Verifies a WOTS signature
 *
 * @param[in] hash A signed hash
 * @param[in] signature The WOTS signature
 * @param[in] public_key The WOTS public key
 *
 * @return true if valid, false otherwise
 */
bool mam_wots_verify(trits_t const hash, trits_t const signature, trits_t const public_key);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_WOTS_WOTS_H__

/** @} */
