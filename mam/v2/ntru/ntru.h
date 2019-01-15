
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file ntru.h
\brief MAM2 NTRU layer.
*/
#ifndef __MAM_V2_NTRU_NTRU_H__
#define __MAM_V2_NTRU_NTRU_H__

#include "mam/v2/defs.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"

/*! \brief NTRU public key - 3g(x)/(1+3f(x)) - size. */
#define MAM2_NTRU_PK_SIZE 9216
/*! \brief NTRU private key - f(x) - size. */
#define MAM2_NTRU_SK_SIZE 1024
/*! \brief NTRU session symmetric key size. */
#define MAM2_NTRU_KEY_SIZE 243
/*! \brief NTRU encrypted key size. */
#define MAM2_NTRU_EKEY_SIZE 9216
/*! \brief NTRU id size. */
#define MAM2_NTRU_ID_SIZE 81

typedef struct ntru_s {
  word_t
      *id; /*!< Key id - the first 27 trytes of the corresponding public key.*/
  word_t *sk; /*!< Private key words - small coefficients of polynomial f. */
  void *
      f; /*!< Internal representation (`poly_t`) of a private key: NTT(1+3f). */
} ntru_t;

trits_t ntru_id_trits(ntru_t *n);

trits_t ntru_sk_trits(ntru_t *n);

/*! \brief Load `NTT(1+3f)` from `sk` trits. */
void ntru_load_sk(ntru_t *n);

void ntru_gen(ntru_t *n, /*!< [in] NTRU interface */
              prng_t *p, /*!< [in] PRNG interface */
              trits_t N, /*!< [in] nonce */
              trits_t pk /*!< [out] NTRU public key (serialized NTT of
                            polynomial h=3g/(1+3f)) */
);

void ntru_encr(trits_t pk,   /*!< [in] NTRU public key */
               prng_t *p,    /*!< [in] PRNG interface */
               spongos_t *s, /*!< [in] Spongos interface */
               trits_t K,    /*!< [in] session symmetric key to be encrypted */
               trits_t N,    /*!< [in] nonce */
               trits_t Y     /*!< [out] encrypted K */
);
void ntru_encr_r(trits_t pk, spongos_t *s, trits_t r, trits_t K, trits_t Y);

bool_t ntru_decr(ntru_t *n,    /*!< [in] NTRU interface */
                 spongos_t *s, /*!< [in] Spongos interface */
                 trits_t Y,    /*!< [in] encrypted K */
                 trits_t K     /*!< [out] decrypted session symmetric key */
);

err_t ntru_create(ialloc *a, ntru_t *n);

void ntru_destroy(ialloc *a, ntru_t *n);

#endif  // __MAM_V2_NTRU_NTRU_H__
