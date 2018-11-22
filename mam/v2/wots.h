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
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"

// WOTS public key size
#define MAM2_WOTS_PK_SIZE 243
// WOTS public key flex size
#define MAM2_WOTS_PK_FLEX_SIZE FLEX_TRIT_SIZE_243

// WOTS private key part size
#define MAM2_WOTS_SK_PART_SIZE 162
// WOTS private key part flex size
#define MAM2_WOTS_SK_PART_FLEX_SIZE FLEX_TRIT_SIZE_162

// WOTS private key parts count
#define MAM2_WOTS_SK_PART_COUNT 81

// WOTS private key size
#define MAM2_WOTS_SK_SIZE (MAM2_WOTS_SK_PART_SIZE * MAM2_WOTS_SK_PART_COUNT)
// WOTS private key flex size
#define MAM2_WOTS_SK_FLEX_SIZE FLEX_TRIT_SIZE_13122

// WOTS signed hash value size
#define MAM2_WOTS_HASH_SIZE 234
// WOTS signed hash value flex size
#define MAM2_WOTS_HASH_FLEX_SIZE FLEX_TRIT_SIZE_234

// WOTS signature size
#define MAM2_WOTS_SIG_SIZE MAM2_WOTS_SK_SIZE
// WOTS signature size
#define MAM2_WOTS_SIG_FLEX_SIZE MAM2_WOTS_SK_FLEX_SIZE

// WOTS interface
typedef struct wots_s {
  isponge *sponge;
  flex_trit_t sk[MAM2_WOTS_SK_FLEX_SIZE];
} wots_t;

/**
 * Initializes a WOTS interface with a Sponge
 *
 * @param wots WOTS interface
 * @param sponge Sponge interface
 */
void wots_init(wots_t *const wots, isponge *const sponge);

/**
 * Resets WOTS sponge and SK
 *
 * @param wots WOTS interface
 */
void wots_reset(wots_t *const wots);

/**
 * Generates a WOTS private key
 *
 * @param wots WOTS interface
 * @param prng PRNG interface
 * @param nonce Nonce
 */
void wots_gen_sk(wots_t *const wots, prng_t *const prng,
                 trit_array_t const *const nonce);

/**
 * Generates a WOTS private key
 *
 * @param wots WOTS interface
 * @param prng PRNG interface
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 */
void wots_gen_sk2(wots_t *const wots, prng_t *const prng,
                  trit_array_t const *const nonce1,
                  trit_array_t const *const nonce2);

/**
 * Generates a WOTS private key
 *
 * @param wots WOTS interface
 * @param prng PRNG interface
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 * @param nonce3 Third nonce
 */
void wots_gen_sk3(wots_t *const wots, prng_t *const prng,
                  trit_array_t const *const nonce1,
                  trit_array_t const *const nonce2,
                  trit_array_t const *const nonce3);

/**
 * Computes a WOTS public key
 * Private key must have already been generated
 *
 * @param wots WOTS interface
 * @param pk Public key
 */
void wots_calc_pk(wots_t *const wots, trit_array_p pk);

/**
 * Generates a WOTS signature
 *
 * @param wots WOTS interface
 * @param hash Hash value to be signed
 * @param sig Signature
 */
void wots_sign(wots_t *const wots, trit_array_p const hash, trit_array_p sig);

/**
 * Recovers a WOTS public key from a signature
 *
 * @param sponge Sponge interface
 * @param hash Signed hash value
 * @param sig Signature
 * @param pk Presumed public key
 */
void wots_recover(isponge *const sponge, trit_array_p const hash,
                  trit_array_p const sig, trit_array_p pk);

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
bool wots_verify(isponge *const sponge, trit_array_p const hash,
                 trit_array_p const sig, trit_array_p const pk);

#endif  // __MAM_V2_WOTS_H__
