/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
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

trits_t mam_ntru_pk_id(mam_ntru_pk_t const *const ntru_pk);
trits_t mam_ntru_pk_key(mam_ntru_pk_t const *const ntru_pk);

/**
 * NTRU encryption of a session key
 *
 * @param public_key A NTRU public key
 * @param prng A PRNG interface
 * @param spongos A spongos interface
 * @param session_key A session symmetric key to be encrypted
 * @param nonce A nonce
 * @param encrypted_session_key The encrypted session key
 */
void ntru_pk_encr(mam_ntru_pk_t const *const ntru_pk,
                  mam_prng_t const *const prng, mam_spongos_t *const spongos,
                  trits_t const session_key, trits_t const nonce,
                  trits_t encrypted_session_key);

/**
 * NTRU encryption of a session key
 *
 * @param public_key A NTRU public key
 * @param spongos A spongos interface
 * @param r Pseudo-random trits
 * @param session_key A session symmetric key to be encrypted
 * @param encrypted_session_key The encrypted session key
 */
void ntru_pk_encr_r(mam_ntru_pk_t const *const ntru_pk,
                    mam_spongos_t *const spongos, trits_t const r,
                    trits_t const session_key, trits_t encrypted_session_key);

size_t mam_ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pk_set);
retcode_t mam_ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pk_set,
                                 trits_t *const trits);
retcode_t mam_ntru_pks_deserialize(trits_t *const trits,
                                   mam_ntru_pk_t_set_t *const ntru_pk_set);

/*
 * NTRU secret key
 */

/**
 * Gets public key id trits
 *
 * @param ntru A NTRU interface
 *
 * @return the id trits
 */
static inline trits_t ntru_sk_pk_id(mam_ntru_sk_t const *const ntru) {
  return trits_from_rep(MAM_NTRU_ID_SIZE, ntru->public_key.key);
}

/**
 * Gets public key trits
 *
 * @param ntru A NTRU interface
 *
 * @return the trits
 */
static inline trits_t ntru_sk_pk_key(mam_ntru_sk_t const *const ntru) {
  return trits_from_rep(MAM_NTRU_PK_SIZE, ntru->public_key.key);
}

/**
 * Getssecret key trits
 *
 * @param ntru A NTRU interface
 *
 * @return the secret  key trits
 */
static inline trits_t ntru_sk_key(mam_ntru_sk_t const *const ntru) {
  return trits_from_rep(MAM_NTRU_SK_SIZE, ntru->secret_key);
}

/**
 * Allocates memory for a NTRU interface
 *
 * @param ntru A NTRU interface
 *
 * @return a status code
 */
retcode_t ntru_sk_init(mam_ntru_sk_t *const ntru);

/**
 * Deallocates memory for a NTRU interface
 *
 * @param ntru A NTRU interface
 *
 * @return a status code
 */
retcode_t ntru_sk_destroy(mam_ntru_sk_t *const ntru);

/**
 * Generates a NTRU puublic key
 *
 * @param ntru A NTRU interface
 * @param prng A PRNG interface
 * @param nonce A nonce
 */
void ntru_sk_gen(mam_ntru_sk_t const *const ntru, mam_prng_t const *const prng,
                 trits_t const nonce);

/**
 * NTRU decryption of an encrypted session key
 *
 * @param ntru A NTRU interface
 * @param spongos Aspongos interface
 * @param encrypted_session_key An encrypted session key
 * @param session_key The decrypted session symmetric key
 *
 * @return
 */
bool ntru_sk_decr(mam_ntru_sk_t const *const ntru, mam_spongos_t *const spongos,
                  trits_t const encrypted_session_key, trits_t session_key);

/**
 * NTRU load the sk functional representation from sk
 *
 * @param ntru A NTRU interface
 *
 * @return
 */

void ntru_sk_load(mam_ntru_sk_t *n);

void mam_ntru_sks_destroy(mam_ntru_sk_t_set_t *const ntru_sks);

size_t mam_ntru_sks_serialized_size(mam_ntru_sk_t_set_t const ntru_sk_set);
retcode_t mam_ntru_sks_serialize(mam_ntru_sk_t_set_t const ntru_sk_set,
                                 trits_t *const trits);
retcode_t mam_ntru_sks_deserialize(trits_t *const trits,
                                   mam_ntru_sk_t_set_t *const ntru_sk_set);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_NTRU_NTRU_H__
