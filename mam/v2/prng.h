/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_PRNG_H__
#define __MAM_V2_PRNG_H__

#include "mam/v2/defs.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"

// PRNG key size
#define MAM2_PRNG_KEY_SIZE 243

// PRNG AE destination tryte
#define MAM2_PRNG_DST_SEC_KEY 0
// PRNG WOTS destination tryte
#define MAM2_PRNG_DST_WOTS_KEY 1
// PRNG NTRU destination tryte
#define MAM2_PRNG_DST_NTRU_KEY 2

// PRNG interface
typedef struct prng_s {
  isponge *sponge;
  flex_trit_t key[FLEX_TRIT_SIZE_243];
} prng_t;

/**
 * PRNG default initialization
 *
 * @param prng PRNG interface
 */
void prng_create(prng_t *const prng);

/**
 * PRNG value initialization
 *
 * @param prng PRNG interface
 * @param sponge Sponge interface
 * @param key Key of size MAM2_PRNG_KEY_SIZE
 */
void prng_init(prng_t *const prng, isponge *const sponge,
               flex_trit_t const *const key);

/**
 * PRNG output generation with nonce
 *
 * @param prng PRNG interface
 * @param dest Destination tryte
 * @param nonce Nonce
 * @param output Pseudorandom trits
 */
void prng_gen(prng_t *const prng, uint8_t const dest, trits_t const nonce,
              trits_t output);

/**
 * PRNG output generation with nonce1 || nonce2
 *
 * @param prng PRNG interface
 * @param dest Destination tryte
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 * @param output Pseudorandom trits
 */
void prng_gen2(prng_t *const prng, uint8_t const dest, trits_t const nonce1,
               trits_t const nonce2, trits_t output);

/**
 * PRNG output generation with nonce1 || nonce2 || nonce3
 *
 * @param prng PRNG interface
 * @param dest Destination tryte
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 * @param nonce3 Third nonce
 * @param output Pseudorandom trits
 */
void prng_gen3(prng_t *const prng, uint8_t const dest, trits_t const nonce1,
               trits_t const nonce2, trits_t const nonce3, trits_t output);

/**
 * Cleans PRNG state
 *
 * @param prng PRNG interface
 */
void prng_destroy(prng_t *const prng);

#endif  // __MAM_V2_PRNG_H__
