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
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

// NTRU public key - 3g(x)/(1+3f(x)) - size
#define MAM2_NTRU_PK_SIZE 9216
// NTRU private key - f(x) - size
#define MAM2_NTRU_SK_SIZE 1024
// NTRU session symmetric key size
#define MAM2_NTRU_KEY_SIZE MAM2_SPONGE_KEY_SIZE
// NTRU encrypted key size
#define MAM2_NTRU_EKEY_SIZE 9216
// NTRU id size
#define MAM2_NTRU_ID_SIZE 81

// NTRU layer interface
typedef struct ntru_s {
  // Key id - the first 27 trytes of the corresponding public key
  trit_t *public_key_id;
  // Private key trits - small coefficients of polynomial f
  trit_t *secret_key;
  // Internal representation (`poly_t`) of a private key: NTT(1+3f)
  void *f;
} ntru_t;

/**
 * Allocates memory for a NTRU interface
 *
 * @param ntru A NTRU interface
 *
 * @return a status code
 */
retcode_t ntru_create(ntru_t *const ntru);

/**
 * Deallocates memory for a NTRU interface
 *
 * @param ntru A NTRU interface
 */
void ntru_destroy(ntru_t *const ntru);

/**
 * Gets public key id trits
 *
 * @param ntru A NTRU interface
 *
 * @return the id trits
 */
static inline trits_t ntru_id_trits(ntru_t const *const ntru) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, ntru->public_key_id);
}

/**
 * Getssecret key trits
 *
 * @param ntru A NTRU interface
 *
 * @return the secret  key trits
 */
static inline trits_t ntru_sk_trits(ntru_t const *const ntru) {
  return trits_from_rep(MAM2_NTRU_SK_SIZE, ntru->secret_key);
}

/**
 * Generates a NTRU puublic key
 *
 * @param ntru A NTRU interface
 * @param prng A PRNG interface
 * @param nonce A nonce
 * @param public_key A NTRU public key: serialized NTT of polynomial h=3g/(1+3f)
 */
void ntru_gen(ntru_t const *const ntru, prng_t const *const prng,
              trits_t const nonce, trits_t public_key);

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
void ntru_encr(trits_t const public_key, prng_t const *const prng,
               spongos_t *const spongos, trits_t const session_key,
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
void ntru_encr_r(trits_t const public_key, spongos_t *const spongos,
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
bool ntru_decr(ntru_t const *const ntru, spongos_t *const spongos,
               trits_t const encrypted_session_key, trits_t session_key);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_NTRU_NTRU_H__
