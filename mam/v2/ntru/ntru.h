/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_NTRU_NTRU_H__
#define __MAM_V2_NTRU_NTRU_H__

#include "mam/v2/defs.h"
#include "mam/v2/ntru/ntru_types.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"

#include "utils/memset_safe.h"

// NTRU session symmetric key size
#define MAM_NTRU_KEY_SIZE MAM_SPONGE_KEY_SIZE
// NTRU encrypted key size
#define MAM_NTRU_EKEY_SIZE 9216

#ifdef __cplusplus
extern "C" {
#endif

trits_t mam_ntru_pk_id(mam_ntru_pk_t const *const ntru_pk);
trits_t mam_ntru_pk_trits(mam_ntru_pk_t const *const ntru_pk);

/**
 * Allocates memory for a NTRU interface
 *
 * @param ntru A NTRU interface
 *
 * @return a status code
 */
retcode_t ntru_init(mam_ntru_sk_t *const ntru);

/**
 * Deallocates memory for a NTRU interface
 *
 * @param ntru A NTRU interface
 *
 * @return a status code
 */
retcode_t ntru_destroy(mam_ntru_sk_t *const ntru);

/**
 * Gets public key id trits
 *
 * @param ntru A NTRU interface
 *
 * @return the id trits
 */
static inline trits_t ntru_id_trits(mam_ntru_sk_t const *const ntru) {
  return trits_from_rep(MAM_NTRU_ID_SIZE, ntru->public_key.key);
}

/**
 * Gets public key trits
 *
 * @param ntru A NTRU interface
 *
 * @return the trits
 */
static inline trits_t ntru_pk_trits(mam_ntru_sk_t const *const ntru) {
  return trits_from_rep(MAM_NTRU_PK_SIZE, ntru->public_key.key);
}

/**
 * Getssecret key trits
 *
 * @param ntru A NTRU interface
 *
 * @return the secret  key trits
 */
static inline trits_t ntru_sk_trits(mam_ntru_sk_t const *const ntru) {
  return trits_from_rep(MAM_NTRU_SK_SIZE, ntru->secret_key);
}

/**
 * Generates a NTRU puublic key
 *
 * @param ntru A NTRU interface
 * @param prng A PRNG interface
 * @param nonce A nonce
 */
void ntru_gen(mam_ntru_sk_t const *const ntru, mam_prng_t const *const prng,
              trits_t const nonce);

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
void ntru_encr(trits_t const public_key, mam_prng_t const *const prng,
               mam_spongos_t *const spongos, trits_t const session_key,
               trits_t const nonce, trits_t encrypted_session_key);

/**
 * NTRU encryption of a session key
 *
 * @param public_key A NTRU public key
 * @param spongos A spongos interface
 * @param r Pseudo-random trits
 * @param session_key A session symmetric key to be encrypted
 * @param encrypted_session_key The encrypted session key
 */
void ntru_encr_r(trits_t const public_key, mam_spongos_t *const spongos,
                 trits_t const r, trits_t const session_key,
                 trits_t encrypted_session_key);

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
bool ntru_decr(mam_ntru_sk_t const *const ntru, mam_spongos_t *const spongos,
               trits_t const encrypted_session_key, trits_t session_key);

/**
 * NTRU load the sk functional representation from sk
 *
 * @param ntru A NTRU interface
 *
 * @return
 */

void ntru_load_sk(mam_ntru_sk_t *n);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_NTRU_NTRU_H__
