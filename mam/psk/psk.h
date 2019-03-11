/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_PSK_PSK_H__
#define __MAM_PSK_PSK_H__

#include "common/errors.h"
#include "mam/prng/prng.h"
#include "mam/trits/trits.h"

#define MAM_PSK_ID_SIZE 81
#define MAM_PSK_KEY_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Pre-Shared Key (PSK) is a secret key of Authenticated Encryption (AE)
 * It is preliminarily transmitted between the entities and is beyond the scope
 * of MAM
 * The PSK id is an identifier of a group of recipients who share the same PSK
 */
typedef struct mam_psk_s {
  trit_t id[MAM_PSK_ID_SIZE];
  trit_t key[MAM_PSK_KEY_SIZE];
} mam_psk_t;

typedef struct mam_psk_t_set_entry_s mam_psk_t_set_entry_t;
typedef mam_psk_t_set_entry_t* mam_psk_t_set_t;

/**
 * Generates a pre-shared key with an id and a nonce
 *
 * @param psk The pre-shared key
 * @param prng A PRNG interface
 * @param id The pre-shared key id (27 trytes)
 * @param nonce A trytes nonce
 * @param nonce_length Length of the trytes nonce
 *
 * @return a status code
 */
retcode_t mam_psk_gen(mam_psk_t* const psk, mam_prng_t const* const prng,
                      tryte_t const* const id, tryte_t const* const nonce,
                      size_t const nonce_length);

/**
 * Safely destroys a pre-shared key by clearing its secret part
 *
 * @param psk The pre-shared key
 */
void mam_psk_destroy(mam_psk_t* const psk);

/**
 * Gets a pre-shared key id trits
 *
 * @param psk The pre-shared key
 *
 * @return the pre-shared key id trits
 */
trits_t mam_psk_id(mam_psk_t const* const psk);

/**
 * Gets a pre-shared key trits
 *
 * @param psk The pre-shared key
 *
 * @return the pre-shared key trits
 */
trits_t mam_psk_key(mam_psk_t const* const psk);

/**
 * Safely destroys a set of pre-shared keys by clearing their secret part and
 * releasing memory
 *
 * @param psks The set of pre-shared keys
 */
void mam_psks_destroy(mam_psk_t_set_t* const psks);

/**
 * Gets the size of a serialized set of pre-shared keys
 *
 * @param psks The set of pre-shared keys
 *
 * @return the serialized size
 */
size_t mam_psks_serialized_size(mam_psk_t_set_t const psks);

/**
 * Serializes a set of pre-shared keys into a trits buffer
 *
 * @param psks The set of pre-shared keys
 * @param trits The trits buffer to serialize into
 *
 * @return a status code
 */
retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t* const trits);

/**
 * Deserializes a set of pre-shared keys from a trits buffer
 *
 * @param trits The trits buffer to deserialize from
 * @param psks The set of pre-shared keys
 *
 * @return a status code
 */
retcode_t mam_psks_deserialize(trits_t* const trits,
                               mam_psk_t_set_t* const psks);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_PSK_PSK_H__
