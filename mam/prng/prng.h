/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_PRNG_PRNG_H__
#define __MAM_PRNG_PRNG_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/trits/trits.h"

// PRNG secret key size
#define MAM_PRNG_KEY_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mam_prng_destination_tryte_e {
  // PRNG AE destination tryte
  MAM_PRNG_DST_SEC_KEY = 0,
  // PRNG WOTS destination tryte
  MAM_PRNG_DST_WOTS_KEY = 1,
  // PRNG NTRU destination tryte
  MAM_PRNG_DST_NTRU_KEY = 2
} mam_prng_destination_tryte_t;

// PRNG layer interface
typedef struct mam_prng_s {
  trit_t secret_key[MAM_PRNG_KEY_SIZE];
} mam_prng_t;

/**
 * PRNG initialization
 *
 * @param prng A PRNG interface
 * @param secret_key A secret key of size MAM_PRNG_KEY_SIZE
 */
void mam_prng_init(mam_prng_t *const prng, trits_t const secret_key);

/**
 * PRNG deinitialization
 *
 * @param prng A PRNG interface
 */
void mam_prng_destroy(mam_prng_t *const prng);

/**
 * PRNG output generation with a nonce
 *
 * @param prng A PRNG interface
 * @param destination A destination tryte
 * @param nonce The nonce
 * @param output Pseudorandom output trits
 */
void mam_prng_gen(mam_prng_t const *const prng,
                  mam_prng_destination_tryte_t const destination,
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
void mam_prng_gen2(mam_prng_t const *const prng,
                   mam_prng_destination_tryte_t const destination,
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
void mam_prng_gen3(mam_prng_t const *const prng,
                   mam_prng_destination_tryte_t const destination,
                   trits_t const nonce1, trits_t const nonce2,
                   trits_t const nonce3, trits_t output);

size_t mam_prng_serialized_size();
void mam_prng_serialize(mam_prng_t const *const prng, trits_t trits);
retcode_t mam_prng_deserialize(trits_t const trits, mam_prng_t *const prng);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_PRNG_PRNG_H__
