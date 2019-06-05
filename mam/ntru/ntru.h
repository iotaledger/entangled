/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __MAM_NTRU_NTRU_H__
#define __MAM_NTRU_NTRU_H__

#include "mam/defs.h"
#include "mam/ntru/mam_ntru_pk_t_set.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/ntru/ntru_types.h"
#include "mam/prng/prng.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NTRU public key
 */

/**
 * Gets a NTRU public key id trits
 *
 * @param ntru_pk The NTRU public key
 *
 * @return the NTRU public key id trits
 */
static inline trits_t mam_ntru_pk_id(mam_ntru_pk_t const *const ntru_pk) {
  return trits_from_rep(MAM_NTRU_ID_SIZE, ntru_pk->key);
}

/**
 * Gets a NTRU public key trits
 *
 * @param ntru_pk The NTRU public key
 *
 * @return the NTRU public key trits
 */
static inline trits_t mam_ntru_pk_key(mam_ntru_pk_t const *const ntru_pk) {
  return trits_from_rep(MAM_NTRU_PK_SIZE, ntru_pk->key);
}

/**
 * NTRU encryption of a session key
 *
 * @param ntru_pk A NTRU public key
 * @param prng A PRNG interface
 * @param spongos A spongos interface
 * @param nonce A nonce
 * @param session_key A session symmetric key to be encrypted
 * @param encrypted_session_key The encrypted session key
 *
 * @return a status code
 */
retcode_t ntru_pk_encr(mam_ntru_pk_t const *const ntru_pk, mam_prng_t const *const prng, mam_spongos_t *const spongos,
                       trits_t const nonce, trits_t const session_key, trits_t encrypted_session_key);

/**
 * Gets the size of a serialized set of NTRU public keys
 *
 * @param ntru_pks The set of NTRU public keys
 *
 * @return the serialized size
 */
size_t mam_ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pks);

/**
 * Serializes a set of NTRU public keys into a trits buffer
 *
 * @param ntru_pks The set of NTRU public keys
 * @param trits The trits buffer to serialize into
 *
 * @return a status code
 */
retcode_t mam_ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pks, trits_t *const trits);

/**
 * Deserializes a set of NTRU public keys from a trits buffer
 *
 * @param trits The trits buffer to deserialize from
 * @param ntru_pks The set of NTRU public keys
 *
 * @return a status code
 */
retcode_t mam_ntru_pks_deserialize(trits_t *const trits, mam_ntru_pk_t_set_t *const ntru_pks);

/*
 * NTRU secret key
 */

/**
 * Gets a NTRU secret key's public key id trits
 *
 * @param ntru_sk The NTRU secret key
 *
 * @return the NTRU secret key's public key id trits
 */
static inline trits_t ntru_sk_pk_id(mam_ntru_sk_t const *const ntru_sk) {
  return trits_from_rep(MAM_NTRU_ID_SIZE, ntru_sk->public_key.key);
}

/**
 * Gets a NTRU secret key's public key trits
 *
 * @param ntru_sk The NTRU secret key
 *
 * @return the NTRU secret key's public key trits
 */
static inline trits_t ntru_sk_pk_key(mam_ntru_sk_t const *const ntru_sk) {
  return trits_from_rep(MAM_NTRU_PK_SIZE, ntru_sk->public_key.key);
}

/**
 * Gets a NTRU secret key trits
 *
 * @param ntru_sk The NTRU secret key
 *
 * @return the NTRU secret key trits
 */
static inline trits_t ntru_sk_key(mam_ntru_sk_t const *const ntru_sk) {
  return trits_from_rep(MAM_NTRU_SK_SIZE, ntru_sk->secret_key);
}

/**
 * Safely resets a NTRU secret key by clearing its secret part
 *
 * @param ntru_sk The NTRU secret key
 *
 * @return a status code
 */
retcode_t ntru_sk_reset(mam_ntru_sk_t *const ntru_sk);

/**
 * Generates a NTRU secret key
 *
 * @param ntru_sk The NTRU secret key
 * @param prng A PRNG interface
 * @param nonce A nonce
 */
void ntru_sk_gen(mam_ntru_sk_t const *const ntru_sk, mam_prng_t const *const prng, trits_t const nonce);

/**
 * NTRU decryption of an encrypted session key
 *
 * @param ntru_sk A NTRU secret key
 * @param spongos A spongos interface
 * @param encrypted_session_key An encrypted session key
 * @param session_key The decrypted session symmetric key
 *
 * @return true if decryption succeeded,false otherwise
 */
bool ntru_sk_decr(mam_ntru_sk_t const *const ntru_sk, mam_spongos_t *const spongos, trits_t const encrypted_session_key,
                  trits_t session_key);

/**
 * Loads the internal representation of a NTRU secret key
 *
 * @param ntru_sk A NTRU secret key
 */
void ntru_sk_load(mam_ntru_sk_t *const ntru_sk);

/**
 * Safely destroys a set of NTRU secret keys by clearing their secret part and
 * releasing memory
 *
 * @param ntru_pks The set of NTRU public keys
 */
void mam_ntru_sks_destroy(mam_ntru_sk_t_set_t *const ntru_sks);

/**
 * Gets the size of a serialized set of NTRU secret keys
 *
 * @param ntru_sks The set of NTRU secret keys
 *
 * @return the serialized size
 */
size_t mam_ntru_sks_serialized_size(mam_ntru_sk_t_set_t const ntru_sks);

/**
 * Serializes a set of NTRU secret keys into a trits buffer
 *
 * @param ntru_sks The set of NTRU secret keys
 * @param trits The trits buffer to serialize into
 *
 * @return a status code
 */
retcode_t mam_ntru_sks_serialize(mam_ntru_sk_t_set_t const ntru_sks, trits_t *const trits);

/**
 * Deserializes a set of NTRU secret keys from a trits buffer
 *
 * @param trits The trits buffer to deserialize from
 * @param ntru_sks The set of NTRU secret keys
 *
 * @return a status code
 */
retcode_t mam_ntru_sks_deserialize(trits_t *const trits, mam_ntru_sk_t_set_t *const ntru_sks);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_NTRU_NTRU_H__

/** @} */
