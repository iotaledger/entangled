/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_NTRU_NTRU_TYPES_H__
#define __MAM_NTRU_NTRU_TYPES_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/ntru/poly.h"
#include "mam/trits/trits.h"

// NTRU public key - 3g(x)/(1+3f(x)) - size
#define MAM_NTRU_PK_SIZE 9216
// NTRU private key - f(x) - size
#define MAM_NTRU_SK_SIZE 1024
// NTRU id size
#define MAM_NTRU_ID_SIZE 81
// NTRU session symmetric key size
#define MAM_NTRU_KEY_SIZE MAM_SPONGE_KEY_SIZE
// NTRU encrypted key size
#define MAM_NTRU_EKEY_SIZE 9216

// Recipient's NTRU public key
typedef struct mam_ntru_pk_s {
  trit_t key[MAM_NTRU_PK_SIZE];
} mam_ntru_pk_t;

typedef struct mam_ntru_pk_t_set_entry_s mam_ntru_pk_t_set_entry_t;
typedef mam_ntru_pk_t_set_entry_t* mam_ntru_pk_t_set_t;

// NTRU layer interface
typedef struct mam_ntru_sk_s {
  // Public key trits
  mam_ntru_pk_t public_key;
  // Private key trits - small coefficients of polynomial f
  trit_t secret_key[MAM_NTRU_SK_SIZE];
  // Internal representation (`poly_t`) of a private key: NTT(1+3f)
  poly_t f;
} mam_ntru_sk_t;

typedef struct mam_ntru_sk_t_set_entry_s mam_ntru_sk_t_set_entry_t;
typedef mam_ntru_sk_t_set_entry_t* mam_ntru_sk_t_set_t;

#endif  // __MAM_NTRU_NTRU_TYPES_H__
