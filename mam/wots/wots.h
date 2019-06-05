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
 * @brief The WOTS layer supports Winternitz One-Time Signatures
 *
 */
#ifndef __MAM_WOTS_WOTS_H__
#define __MAM_WOTS_WOTS_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/prng/prng.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"
#include "utils/memset_safe.h"

/** @brief Size of a WOTS public key */
#define MAM_WOTS_PUBLIC_KEY_SIZE 243
/** @brief Size of a WOTS private key part */
#define MAM_WOTS_PRIVATE_KEY_PART_SIZE 162
/** @brief Number of parts in a WOTS private key */
#define MAM_WOTS_PRIVATE_KEY_PART_COUNT 81
/** @brief Size of a WOTS private key */
#define MAM_WOTS_PRIVATE_KEY_SIZE (MAM_WOTS_PRIVATE_KEY_PART_SIZE * MAM_WOTS_PRIVATE_KEY_PART_COUNT)
/** @brief Size of a WOTS signed hash */
#define MAM_WOTS_HASH_SIZE 234
/** @brief Size of a WOTS signature */
#define MAM_WOTS_SIGNATURE_SIZE MAM_WOTS_PRIVATE_KEY_SIZE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_wots_s {
  trit_t private_key[MAM_WOTS_PRIVATE_KEY_SIZE];
} mam_wots_t;

/**
 * @brief Safely resets a WOTS private key
 *
 * @param[out] wots The WOTS private key
 *
 * @return a status code
 */
static inline retcode_t mam_wots_reset(mam_wots_t *const wots) {
  if (wots == NULL) {
    return RC_NULL_PARAM;
  }

  memset_safe(wots->private_key, MAM_WOTS_PRIVATE_KEY_SIZE, 0, MAM_WOTS_PRIVATE_KEY_SIZE);

  return RC_OK;
}

/**
 * @brief Generates a WOTS private key with five nonces
 *
 * @param[out] wots The WOTS private key
 * @param[in] prng A PRNG
 * @param[in] nonce1 The first nonce
 * @param[in] nonce2 The second nonce
 * @param[in] nonce3 The third nonce
 * @param[in] nonce4 The fourth nonce
 * @param[in] nonce5 The fifth nonce
 *
 * @return a status code
 */
static inline retcode_t mam_wots_gen_sk5(mam_wots_t *const wots, mam_prng_t const *const prng, trits_t const nonce1,
                                         trits_t const nonce2, trits_t const nonce3, trits_t const nonce4,
                                         trits_t const nonce5) {
  if (wots == NULL) {
    return RC_NULL_PARAM;
  }

  return mam_prng_gen5(prng, MAM_PRNG_DST_WOTS_KEY, nonce1, nonce2, nonce3, nonce4, nonce5,
                       trits_from_rep(MAM_WOTS_PRIVATE_KEY_SIZE, wots->private_key));
}

/**
 * @brief Generates a WOTS private key with a nonce
 *
 * @param[out] wots The WOTS private key
 * @param[in] prng A PRNG
 * @param[in] nonce The nonce
 *
 * @return a status code
 */
static inline retcode_t mam_wots_gen_sk(mam_wots_t *const wots, mam_prng_t const *const prng, trits_t const nonce) {
  return mam_wots_gen_sk5(wots, prng, nonce, trits_null(), trits_null(), trits_null(), trits_null());
}

/**
 * @brief Generates a WOTS public key associated with a WOTS private key
 *
 * The private key must have already been generated
 *
 * @param[in] wots The WOTS private key
 * @param[out] public_key The WOTS public key
 *
 * @return a status code
 */
retcode_t mam_wots_gen_pk(mam_wots_t const *const wots, trits_t public_key);

/**
 * @brief Generates a WOTS signature associated with a WOTS private key
 *
 * @param[in] wots The WOTS private key
 * @param[in] hash A hash to be signed
 * @param[out] signature The WOTS signature
 *
 * @return a status code
 */
retcode_t mam_wots_sign(mam_wots_t const *const wots, trits_t const hash, trits_t signature);

/**
 * @brief Recovers a WOTS public key from a WOTS signature
 *
 * @param[in] hash A signed hash
 * @param[in] signature The WOTS signature
 * @param[out] public_key The WOTS public key
 *
 * @return a status code
 */
retcode_t mam_wots_recover(trits_t const hash, trits_t const signature, trits_t public_key);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_WOTS_WOTS_H__

/** @} */
