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
#ifndef __MAM_SPONGE_SPONGOS_H__
#define __MAM_SPONGE_SPONGOS_H__

#include "mam/defs.h"
#include "mam/sponge/sponge.h"
#include "mam/sponge/spongos_types.h"
#include "mam/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a spongos state
 *
 * @param spongos A spongos interface
 */
void mam_spongos_init(mam_spongos_t *const spongos);

/**
 * Creates an equivalent spongos instance
 *
 * @param spongos A spongos interface
 * @param fork The fork
 */
void mam_mam_spongos_fork(mam_spongos_t const *const spongos, mam_spongos_t *const fork);

/**
 * Commits changes in the rate part
 *
 * @param spongos A spongos interface
 */
void mam_spongos_commit(mam_spongos_t *const spongos);

/**
 * Processes input data
 *
 * @param spongos A spongos interface
 * @param input Input data
 */
void mam_spongos_absorb(mam_spongos_t *const spongos, trits_t input);

/**
 * Processes n inputs data
 *
 * @param spongos A spongos interface
 * @param n Number of input data
 * @param inputs Inputs data
 */
void mam_spongos_absorbn(mam_spongos_t *const spongos, size_t const n, trits_t *const inputs);

/**
 * Generates output data
 *
 * @param spongos A spongos interface
 * @param output Output data
 */
void mam_spongos_squeeze(mam_spongos_t *const spongos, trits_t output);

/**
 * Generates output data and check for equality with given output
 *
 * @param spongos A spongos interface
 * @param expected_output Expected output data
 */
bool mam_spongos_squeeze_eq(mam_spongos_t *const spongos, trits_t expected_output);

/**
 * Hashes input data
 *
 * @param spongos A spongos interface
 * @param input Input data
 * @param output Output data
 */
void mam_spongos_hash(mam_spongos_t *const spongos, trits_t input, trits_t output);

/**
 * Hashes n input data
 *
 * @param spongos A spongos interface
 * @param n Number of input data
 * @param inputs Inputs data
 * @param output Output data
 */
void mam_spongos_hashn(mam_spongos_t *const spongos, size_t const n, trits_t *inputs, trits_t output);

/**
 * Encrypts plaintext
 *
 * @param spongos A spongos interface
 * @param plaintext Plaintext input
 * @param ciphertext Ciphertext output
 */
void mam_spongos_encr(mam_spongos_t *const spongos, trits_t plaintext, trits_t ciphertext);

/**
 * Decrypts ciphertext
 *
 * @param spongos A spongos interface
 * @param ciphertext Ciphertext input
 * @param plaintext Plaintext output
 */
void mam_spongos_decr(mam_spongos_t *const spongos, trits_t ciphertext, trits_t plaintext);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_SPONGE_SPONGOS_H__

/** @} */
