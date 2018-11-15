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
#include "mam/v2/err.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"

// PRNG key size
#define MAM2_PRNG_KEY_SIZE 243

typedef trit_t prng_key_t[MAM2_WORDS(MAM2_PRNG_KEY_SIZE)];

// PRNG AE destination tryte
#define MAM2_PRNG_DST_SECKEY 0
// PRNG WOTS destination tryte
#define MAM2_PRNG_DST_WOTSKEY 1
// PRNG NTRU destination tryte
#define MAM2_PRNG_DST_NTRUKEY 2

// PRNG interface
typedef struct prng_s {
  isponge *sponge;
  trit_t *key;
} prng_t;

/**
 * Allocates memory for PRNG secret key
 *
 * @param prng PRNG interface
 */
err_t prng_create(prng_t *prng);

/**
 * PRNG initialization
 *
 * @param prng PRNG interface
 * @param sponge Sponge interface
 * @param key Key of size MAM2_PRNG_KEY_SIZE
 */
void prng_init(prng_t *prng, isponge *sponge, trits_t key);

/**
 * PRNG output generation with nonce
 *
 * @param prng PRNG interface
 * @param dest Destination tryte
 * @param nonce Nonce
 * @param trits Pseudorandom trits
 */
void prng_gen(prng_t *prng, trint3_t dest, trits_t nonce, trits_t trits);

/**
 * PRNG output generation with nonce1 || nonce2
 *
 * @param prng PRNG interface
 * @param dest Destination tryte
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 * @param trits Pseudorandom trits
 */
void prng_gen2(prng_t *prng, trint3_t dest, trits_t nonce1, trits_t nonce2,
               trits_t trits);

/**
 * PRNG output generation with nonce1 || nonce2 || nonce3
 *
 * @param prng PRNG interface
 * @param dest Destination tryte
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 * @param nonce3 Third nonce
 * @param trits Pseudorandom trits
 */
void prng_gen3(prng_t *prng, trint3_t dest, trits_t nonce1, trits_t nonce2,
               trits_t nonce3, trits_t trits);

/**
 * Deallocates memory for PRNG secret key
 *
 * @param prng PRNG interface
 */
void prng_destroy(prng_t *prng);

#endif  // __MAM_V2_PRNG_H__
