/*!
\file ntru.h
\brief MAM2 NTRU layer.
*/
#pragma once

#include "mam/v2/defs.h"
#include "mam/v2/prng.h"
#include "mam/v2/spongos.h"
#include "mam/v2/trits.h"

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

typedef struct _intru {
  word_t
      *id; /*!< Key id - the first 27 trytes of the corresponding public key.*/
  word_t *sk; /*!< Private key words - small coefficients of polynomial f. */
  void *
      f; /*!< Internal representation (`poly_t`) of a private key: NTT(1+3f). */
} intru;

trits_t ntru_id_trits(intru *n);

trits_t ntru_sk_trits(intru *n);

/*! \brief Load `NTT(1+3f)` from `sk` trits. */
void ntru_load_sk(intru *n);

void ntru_gen(intru *n,  /*!< [in] NTRU interface */
              iprng *p,  /*!< [in] PRNG interface */
              trits_t N, /*!< [in] nonce */
              trits_t pk /*!< [out] NTRU public key (serialized NTT of
                            polynomial h=3g/(1+3f)) */
);

void ntru_encr(trits_t pk,  /*!< [in] NTRU public key */
               iprng *p,    /*!< [in] PRNG interface */
               ispongos *s, /*!< [in] Spongos interface */
               trits_t K,   /*!< [in] session symmetric key to be encrypted */
               trits_t N,   /*!< [in] nonce */
               trits_t Y    /*!< [out] encrypted K */
);

bool_t ntru_decr(intru *n,    /*!< [in] NTRU interface */
                 ispongos *s, /*!< [in] Spongos interface */
                 trits_t Y,   /*!< [in] encrypted K */
                 trits_t K    /*!< [out] decrypted session symmetric key */
);

err_t ntru_create(ialloc *a, intru *n);

void ntru_destroy(ialloc *a, intru *n);

bool_t ntru_test(intru *n, ispongos *s, iprng *p);
