/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_PRNG_PRNG_H__
#define __MAM_V2_PRNG_PRNG_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/prng/prng_types.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates memory for PRNG secret key
 *
 * @param prng A PRNG interface
 *
 * @return a status code
 */
retcode_t mam_prng_create(mam_prng_t *const prng);

/**
 * Deallocates memory for PRNG secret key
 *
 * @param prng A PRNG interface
 */
void mam_prng_destroy(mam_prng_t *const prng);

/**
 * PRNG initialization
 *
 * @param prng A PRNG interface
 * @param sponge A sponge interface
 * @param secret_key A secret key of size MAM2_PRNG_KEY_SIZE
 */
void mam_prng_init(mam_prng_t *const prng, mam_sponge_t *const sponge,
                   trits_t const secret_key);

/**
 * PRNG output generation with a nonce
 *
 * @param prng A PRNG interface
 * @param destination A destination tryte
 * @param nonce The nonce
 * @param output Pseudorandom output trits
 */
void mam_prng_gen(mam_prng_t const *const prng, tryte_t const destination,
                  trits_t const nonce, trits_t output);

/**
 * PRNG output generation with two nonces
 *
 * @param prng A PRNG interface
 * @param destination A destination tryte
 * @param nonce1 The first nonce
 * @param nonce2 The second nonce
 * @param output Pseudorandom output trits
 */
void mam_prng_gen2(mam_prng_t const *const prng, tryte_t const destination,
                   trits_t const nonce1, trits_t const nonce2, trits_t output);

/**
 * PRNG output generation with three nonces
 *
 * @param prng A PRNG interface
 * @param destination A destination tryte
 * @param nonce1 The first nonce
 * @param nonce2 The second nonce
 * @param nonce3 The third nonce
 * @param output Pseudorandom output trits
 */
void mam_prng_gen3(mam_prng_t const *const prng, tryte_t const destination,
                   trits_t const nonce1, trits_t const nonce2,
                   trits_t const nonce3, trits_t output);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_PRNG_PRNG_H__
