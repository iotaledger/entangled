/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_WOTS_H__
#define __MAM_V2_WOTS_H__

#include "mam/v2/defs.h"
#include "mam/v2/err.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"

// WOTS public key size
#define MAM2_WOTS_PK_SIZE 243
// WOTS private key part size
#define MAM2_WOTS_SK_PART_SIZE 162
// WOTS private key parts count
#define MAM2_WOTS_SK_PART_COUNT 81
// WOTS private key size
#define MAM2_WOTS_SK_SIZE (MAM2_WOTS_SK_PART_SIZE * MAM2_WOTS_SK_PART_COUNT)
// WOTS signed hash value size
#define MAM2_WOTS_HASH_SIZE 234
// WOTS signature size
#define MAM2_WOTS_SIG_SIZE MAM2_WOTS_SK_SIZE

typedef trit_t wots_sk_t[MAM2_WORDS(MAM2_WOTS_SK_SIZE)];

// WOTS interface
typedef struct wots_s {
  isponge *sponge;
  trit_t *sk;
} wots_t;

/**
 * Initializes a WOTS interface with a Sponge
 *
 * @param wots WOTS interface
 * @param sponge Sponge interface
 */
void wots_init(wots_t *wots, isponge *sponge);

/**
 * Generates a WOTS private key
 *
 * @param wots WOTS interface
 * @param prng PRNG interface
 * @param nonce Nonce
 */
void wots_gen_sk(wots_t *wots, prng_t *prng, trits_t nonce);

/**
 * Generates a WOTS private key
 *
 * @param wots WOTS interface
 * @param prng PRNG interface
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 */
void wots_gen_sk2(wots_t *wots, prng_t *prng, trits_t nonce1, trits_t nonce2);

/**
 * Generates a WOTS private key
 *
 * @param wots WOTS interface
 * @param prng PRNG interface
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 * @param nonce3 Third nonce
 */
void wots_gen_sk3(wots_t *wots, prng_t *prng, trits_t nonce, trits_t nonce2,
                  trits_t nonce3);

/**
 * Computes a WOTS public key
 * Private key must have already been generated
 *
 * @param wots WOTS interface
 * @param pk Public key
 */
void wots_calc_pk(wots_t *wots, trits_t pk);

/**
 * Generates a WOTS signature
 *
 * @param wots WOTS interface
 * @param hash Hash value to be signed
 * @param sig Signature
 */
void wots_sign(wots_t *wots, trits_t hash, trits_t sig);

/**
 * Recovers a WOTS public key from a signature
 *
 * @param sponge Sponge interface
 * @param hash Signed hash value
 * @param sig Signature
 * @param pk Presumed public key
 */
void wots_recover(isponge *sponge, trits_t hash, trits_t sig, trits_t pk);

/**
 * Verifies a WOTS signature
 *
 * @param sponge Sponge interface
 * @param hash Signed hash value
 * @param sig Signature
 * @param pk Public key
 *
 * @return true if signature is valid, false otherwise
 */
bool_t wots_verify(isponge *sponge, trits_t hash, trits_t sig, trits_t pk);

/**
 * Allocates memory for WOTS private key
 *
 * @param wots WOTS interface
 *
 * @return a status code
 */
err_t wots_create(wots_t *wots);

/**
 * Deallocates memory for WOTS private key
 *
 * @param wots WOTS interface
 */
void wots_destroy(wots_t *wots);

/**
 * Gets the WOTS private key
 *
 * @param wots WOTS interface
 *
 * @return the WOTS private key
 */
trits_t wots_sk_trits(wots_t *wots);

#endif  // __MAM_V2_WOTS_H__
