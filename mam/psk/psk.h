/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief A Pre-Shared Key (PSK) is a secret key of Authenticated Encryption (AE)
 *
 * It is preliminarily transmitted between the entities and is beyond the scope of MAM
 * The PSK id is an identifier of a group of recipients who share the same PSK
 *
 */
#ifndef __MAM_PSK_PSK_H__
#define __MAM_PSK_PSK_H__

#include "common/errors.h"
#include "mam/prng/prng.h"
#include "mam/trits/trits.h"
#include "utils/memset_safe.h"

/** @brief Size of a Pre-Shared Key ID */
#define MAM_PSK_ID_SIZE 81
/** @brief Size of a Pre-Shared Key */
#define MAM_PSK_KEY_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_psk_s {
  trit_t id[MAM_PSK_ID_SIZE];
  trit_t key[MAM_PSK_KEY_SIZE];
} mam_psk_t;

typedef struct mam_psk_t_set_entry_s mam_psk_t_set_entry_t;
typedef mam_psk_t_set_entry_t* mam_psk_t_set_t;

/**
 * @brief Generates a pre-shared key with an id and a nonce
 *
 * @param[out] psk The pre-shared key
 * @param[in] prng A PRNG
 * @param[in] id The pre-shared key id
 * @param[in] nonce A trytes nonce
 * @param[in] nonce_length Length of the trytes nonce
 *
 * @return a status code
 */
retcode_t mam_psk_gen(mam_psk_t* const psk, mam_prng_t const* const prng, tryte_t const* const id,
                      tryte_t const* const nonce, size_t const nonce_length);

/**
 * @brief Safely resets a pre-shared key by clearing its secret part
 *
 * @param[out] psk The pre-shared key
 *
 * @return a status code
 */
static inline retcode_t mam_psk_reset(mam_psk_t* const psk) {
  if (psk == NULL) {
    return RC_NULL_PARAM;
  }

  memset_safe(psk->key, MAM_PSK_KEY_SIZE, 0, MAM_PSK_KEY_SIZE);

  return RC_OK;
}

/**
 * @brief Gets a pre-shared key id trits
 *
 * @param[in] psk The pre-shared key
 *
 * @return the pre-shared key id trits
 */
static inline trits_t mam_psk_id(mam_psk_t const* const psk) {
  if (psk == NULL) {
    return trits_null();
  }

  return trits_from_rep(MAM_PSK_ID_SIZE, psk->id);
}

/**
 * @brief Gets a pre-shared key trits
 *
 * @param[in] psk The pre-shared key
 *
 * @return the pre-shared key trits
 */
static inline trits_t mam_psk_key(mam_psk_t const* const psk) {
  if (psk == NULL) {
    return trits_null();
  }

  return trits_from_rep(MAM_PSK_KEY_SIZE, psk->key);
}

/**
 * @brief Safely destroys a set of pre-shared keys by clearing their secret part and releasing memory
 *
 * @param[out] psks The set of pre-shared keys
 *
 * @return a status code
 */
retcode_t mam_psks_destroy(mam_psk_t_set_t* const psks);

/**
 * @brief Gets the size of a serialized set of pre-shared keys
 *
 * @param[in] psks The set of pre-shared keys
 *
 * @return the serialized size
 */
size_t mam_psks_serialized_size(mam_psk_t_set_t const psks);

/**
 * @brief Serializes a set of pre-shared keys into a trits buffer
 *
 * @param[in] psks The set of pre-shared keys
 * @param[out] trits The trits buffer to serialize into
 *
 * @return a status code
 */
retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t* const trits);

/**
 * @brief Deserializes a set of pre-shared keys from a trits buffer
 *
 * @param trits[in] The trits buffer to deserialize from
 * @param psks[out] The set of pre-shared keys
 *
 * @return a status code
 */
retcode_t mam_psks_deserialize(trits_t* const trits, mam_psk_t_set_t* const psks);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_PSK_PSK_H__

/** @} */
