/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_WOTS_WOTS_H__
#define __MAM_V2_WOTS_WOTS_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"

// WOTS public key size
#define MAM2_WOTS_PK_SIZE 243
// WOTS secret key part size
#define MAM2_WOTS_SK_PART_SIZE 162
// WOTS secret key parts count
#define MAM2_WOTS_SK_PART_COUNT 81
// WOTS secret key size
#define MAM2_WOTS_SK_SIZE (MAM2_WOTS_SK_PART_SIZE * MAM2_WOTS_SK_PART_COUNT)
// WOTS signed hash value size
#define MAM2_WOTS_HASH_SIZE 234
// WOTS signature size
#define MAM2_WOTS_SIG_SIZE MAM2_WOTS_SK_SIZE

#ifdef __cplusplus
extern "C" {
#endif

// WOTS layer interface
typedef struct wots_s {
  spongos_t spongos;
  trit_t *secret_key;
} wots_t;

/**
 * Allocates memory for WOTS secret key
 *
 * @param wots A WOTS interface
 *
 * @return a status code
 */
retcode_t wots_create(wots_t *const wots);

/**
 * Deallocates memory for WOTS secret key
 *
 * @param wots A WOTS interface
 */
void wots_destroy(wots_t *const wots);

/**
 * Initializes a WOTS interface with a sponge
 *
 * @param wots A WOTS interface
 * @param sponge A sponge interface
 */
void wots_init(wots_t *const wots, sponge_t *const sponge);

/**
 * Generates a WOTS secret key with a nonce
 *
 * @param wots A WOTS interface
 * @param prng A PRNG interface
 * @param nonce The nonce
 */
void wots_gen_sk(wots_t *const wots, prng_t const *const prng,
                 trits_t const nonce);

/**
 * Generates a WOTS secret key with two nonces
 *
 * @param wots A WOTS interface
 * @param prng A PRNG interface
 * @param nonce1 The first nonce
 * @param nonce2 The second nonce
 */
void wots_gen_sk2(wots_t *const wots, prng_t const *const prng,
                  trits_t const nonce1, trits_t const nonce2);

/**
 * Generate a WOTS secret key with three nonces
 *
 * @param wots A WOTS interface
 * @param prng A PRNG interface
 * @param nonce1 The first nonce
 * @param nonce2 The second nonce
 * @param nonce3 The third nonce
 */
void wots_gen_sk3(wots_t *const wots, prng_t const *const prng,
                  trits_t const nonce1, trits_t const nonce2,
                  trits_t const nonce3);

/**
 * Calculates a WOTS public key
 * Private key must have already been generated
 *
 * @param wots A WOTS interface
 * @param public_key The public key
 */
void wots_calc_pk(wots_t *const wots, trits_t public_key);

/**
 * Generates a WOTS signature
 *
 * @param wots A WOTS interface
 * @param hash A hash to be signed
 * @param signature The signature
 */
void wots_sign(wots_t *const wots, trits_t const hash, trits_t signature);

/**
 * Recovers a WOTS public key from a signature
 *
 * @param spongos A spongos interface
 * @param hash A signed hash
 * @param signature The signature
 * @param public_key The recovered public key
 */
void wots_recover(spongos_t *const spongos, trits_t const hash,
                  trits_t const signature, trits_t public_key);

/**
 * Verifies a WOTS signature
 *
 * @param spongos A spongos interface
 * @param hash A signed hash
 * @param signature The signature
 * @param public_key The presumed public key
 *
 * @return true if valid, false otherwise
 */
bool wots_verify(spongos_t *const spongos, trits_t const hash,
                 trits_t const signature, trits_t const public_key);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_WOTS_WOTS_H__
