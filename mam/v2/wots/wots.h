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
typedef struct mam_wots_s {
  mam_spongos_t spongos;
  trit_t secret_key[MAM2_WOTS_SK_SIZE];
} mam_wots_t;

/**
 * Allocates memory for WOTS secret key
 *
 * @param wots A WOTS interface
 *
 * @return a status code
 */
retcode_t mam_wots_create(mam_wots_t *const wots);

/**
 * Deallocates memory for WOTS secret key
 *
 * @param wots A WOTS interface
 */
void mam_wots_destroy(mam_wots_t *const wots);

/**
 * Initializes a WOTS interface with a sponge
 *
 * @param wots A WOTS interface
 * @param sponge A sponge interface
 */
void mam_wots_init(mam_wots_t *const wots, mam_sponge_t *const sponge);

/**
 * Generates a WOTS secret key with a nonce
 *
 * @param wots A WOTS interface
 * @param prng A PRNG interface
 * @param nonce The nonce
 */

void mam_wots_gen_sk(mam_wots_t *const wots, mam_prng_t const *const prng,
                     trits_t const nonce);

/**
 * Generates a WOTS secret key with two nonces
 *
 * @param wots A WOTS interface
 * @param prng A PRNG interface
 * @param nonce1 The first nonce
 * @param nonce2 The second nonce
 */
void mam_wots_gen_sk2(mam_wots_t *const wots, mam_prng_t const *const prng,
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
void mam_wots_gen_sk3(mam_wots_t *const wots, mam_prng_t const *const prng,
                      trits_t const nonce1, trits_t const nonce2,
                      trits_t const nonce3);

/**
 * Calculates a WOTS public key
 * Private key must have already been generated
 *
 * @param wots A WOTS interface
 * @param public_key The public key
 */
void mam_wots_calc_pk(mam_wots_t *const wots, trits_t public_key);

/**
 * Generates a WOTS signature
 *
 * @param wots A WOTS interface
 * @param hash A hash to be signed
 * @param signature The signature
 */
void mam_wots_sign(mam_wots_t *const wots, trits_t const hash,
                   trits_t signature);

/**
 * Recovers a WOTS public key from a signature
 *
 * @param spongos A spongos interface
 * @param hash A signed hash
 * @param signature The signature
 * @param public_key The recovered public key
 */
void mam_wots_recover(mam_spongos_t *const spongos, trits_t const hash,
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
bool mam_wots_verify(mam_spongos_t *const spongos, trits_t const hash,
                     trits_t const signature, trits_t const public_key);

size_t mam_wots_serialized_size(mam_wots_t const *const wots);

void mam_wots_serialize(mam_wots_t const *const wots, trits_t trits);

void mam_mam_wots_deserialize(trits_t const *const trits,
                              mam_wots_t *const wots);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_WOTS_WOTS_H__
