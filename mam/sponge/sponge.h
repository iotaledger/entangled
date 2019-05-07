/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
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
#ifndef __MAM_SPONGE_SPONGE_H__
#define __MAM_SPONGE_SPONGE_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/trits/trits.h"

// Sponge state rate
#define MAM_SPONGE_RATE 486
// Sponge state control
#define MAM_SPONGE_CONTROL 6
// Sponge state capacity
#define MAM_SPONGE_CAPACITY 237
// Sponge state width
#define MAM_SPONGE_WIDTH (MAM_SPONGE_RATE + MAM_SPONGE_CONTROL + MAM_SPONGE_CAPACITY)

// Sponge fixed key size
#define MAM_SPONGE_KEY_SIZE 243
// Sponge fixed hash size
#define MAM_SPONGE_HASH_SIZE 243
// Sponge fixed MAC size
#define MAM_SPONGE_MAC_SIZE 243

// c2 control trit DATA
#define MAM_SPONGE_CTL_DATA 0
// c2 control trit HASH
#define MAM_SPONGE_CTL_HASH 0
// c2 control trit KEY
#define MAM_SPONGE_CTL_KEY 1
// c2 control trit PRN
#define MAM_SPONGE_CTL_PRN 1
// c2 control trit TEXT
#define MAM_SPONGE_CTL_TEXT -1
// c2 control trit MAC
#define MAM_SPONGE_CTL_MAC -1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sponge interface
 *
 * @field stack Additional memory used by the transformation function
 * @field state sponge state
 */
typedef struct sponge_s {
  trit_t state[MAM_SPONGE_WIDTH];
} mam_sponge_t;

/**
 * Gets part (the first `rate` trits) of the sponge outer state
 *
 * @param sponge Sponge interface
 *
 * @return the trits
 */
trits_t mam_sponge_outer_trits(mam_sponge_t const *const sponge);

/**
 * Sponge state initialization
 *
 * @param sponge Sponge interface
 */
void mam_sponge_init(mam_sponge_t *const sponge);

/**
 * Internal state transformation
 *
 * @param sponge Sponge interface
 */
void mam_sponge_transform(mam_sponge_t *const sponge);

/**
 * Fork (copy) sponge state. `fork` must be initialized
 *
 * @param sponge Sponge interface
 * @param fork Sponge interface
 */
void mam_sponge_fork(mam_sponge_t const *const sponge, mam_sponge_t *const fork);

/**
 * Sponge absorption
 *
 * @param sponge Sponge interface
 * @param c2 Control trit encoding output data type
 * @param data Input data blocks
 */
void mam_sponge_absorb(mam_sponge_t *const sponge, trit_t const c2, trits_t data);
/**
 * Absorb concatenation of `Xs[0]`..`Xs[n-1]`
 *
 * @param sponge Sponge interface
 * @param c2 Control trit encoding output data type
 * @param n Input data blocks count
 * @param data_blocks Input data blocks
 */
void mam_sponge_absorbn(mam_sponge_t *const sponge, trit_t const c2, size_t const n, trits_t const *const data_blocks);
/**
 * Sponge squeezing
 *
 * @param sponge Sponge interface
 * @param c2 Control trit encoding output data type
 * @param squeezed Output data
 */
void mam_sponge_squeeze(mam_sponge_t *const sponge, trit_t const c2, trits_t squeezed);
/**
 * Sponge AE encryption
 *
 * @param sponge Sponge interface
 * @param plaintext Input data
 * @param ciphertext Hash value
 */
void mam_sponge_encr(mam_sponge_t *const sponge, trits_t plaintext, trits_t ciphertext);
/**
 * Sponge AE decryption
 *
 * @param sponge Sponge interface
 * @param ciphertext Hash value
 * @param plaintext Input data
 */
void mam_sponge_decr(mam_sponge_t *const sponge, trits_t ciphertext, trits_t plaintext);
/**
 * Sponge hashing
 *
 * @param sponge Sponge interface
 * @param plaintext Input data
 * @param digest Hash value
 */
void mam_sponge_hash(mam_sponge_t *const sponge, trits_t const plaintext, trits_t digest);

/**
 * Sponge hashing
 *
 * @param sponge Sponge interface
 * @param n Input data blocks count
 * @param plaintext_blocks Input data blocks
 * @param digest Hash value
 */
void mam_sponge_hashn(mam_sponge_t *const sponge, size_t const n, trits_t const *const plaintext_blocks,
                      trits_t digest);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_SPONGE_SPONGE_H__

/** @} */
