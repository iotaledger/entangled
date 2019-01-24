/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_SPONGE_SPONGOS_H__
#define __MAM_V2_SPONGE_SPONGOS_H__

#include "mam/v2/defs.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spongos_s {
  sponge_t *sponge;
  size_t pos;
} spongos_t;

/**
 * Initializes a spongos state
 *
 * @param spongos A spongos interface
 */
void spongos_init(spongos_t *const spongos);

/**
 * Creates an equivalent spongos instance
 *
 * @param spongos A spongos interface
 * @param fork The fork
 */
void spongos_fork(spongos_t const *const spongos, spongos_t *const fork);

/**
 * Commits changes in the rate part
 *
 * @param spongos A spongos interface
 */
void spongos_commit(spongos_t *const spongos);

/**
 * Processes input data
 *
 * @param spongos A spongos interface
 * @param input Input data
 */
void spongos_absorb(spongos_t *const spongos, trits_t input);

/**
 * Processes n inputs data
 *
 * @param spongos A spongos interface
 * @param n Number of input data
 * @param inputs Inputs data
 */
void spongos_absorbn(spongos_t *const spongos, size_t const n,
                     trits_t *const inputs);

/**
 * Generates output data
 *
 * @param spongos A spongos interface
 * @param output Output data
 */
void spongos_squeeze(spongos_t *const spongos, trits_t output);

/**
 * Generates output data and check for equality with given output
 *
 * @param spongos A spongos interface
 * @param output Output data
 */
bool spongos_squeeze_eq(spongos_t *const spongos, trits_t output);

/**
 * Hashes input data
 *
 * @param spongos A spongos interface
 * @param input Input data
 * @param output Output data
 */
void spongos_hash(spongos_t *const spongos, trits_t input, trits_t output);

/**
 * Hashes n input data
 *
 * @param spongos A spongos interface
 * @param n Number of input data
 * @param inputs Inputs data
 * @param output Output data
 */
void spongos_hashn(spongos_t *const spongos, size_t const n, trits_t *inputs,
                   trits_t output);

/**
 * Encrypts plaintext
 *
 * @param spongos A spongos interface
 * @param input plaintext input
 * @param output Encrypted output
 */
void spongos_encr(spongos_t *const spongos, trits_t input, trits_t output);

/**
 * Decrypts cyphertext
 *
 * @param spongos A spongos interface
 * @param input Cyphertext input
 * @param output Decrypted output
 */
void spongos_decr(spongos_t *const spongos, trits_t input, trits_t output);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_SPONGE_SPONGOS_H__
