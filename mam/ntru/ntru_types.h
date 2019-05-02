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
#ifndef __MAM_NTRU_NTRU_TYPES_H__
#define __MAM_NTRU_NTRU_TYPES_H__

#include "mam/defs.h"
#include "mam/ntru/poly.h"

// NTRU id size
#define MAM_NTRU_ID_SIZE 81
// NTRU public key size
#define MAM_NTRU_PK_SIZE 9216
// NTRU secret key size
#define MAM_NTRU_SK_SIZE 1024
// NTRU session symmetric key size
#define MAM_NTRU_KEY_SIZE MAM_SPONGE_KEY_SIZE
// NTRU encrypted key size
#define MAM_NTRU_EKEY_SIZE 9216

/**
 * The NTRU layer supports an NTRU-style public key encryption scheme
 * Using NTRU a sender can encrypt session keys with a public key of a recipient
 * The secret key must be kept in secret
 * The corresponding public key, on the contrary, is publicly announced
 */

// Receiver's NTRU public key
typedef struct mam_ntru_pk_s {
  trit_t key[MAM_NTRU_PK_SIZE];
} mam_ntru_pk_t;

typedef struct mam_ntru_pk_t_set_entry_s mam_ntru_pk_t_set_entry_t;
typedef mam_ntru_pk_t_set_entry_t* mam_ntru_pk_t_set_t;

// Sender's NTRU secret key
typedef struct mam_ntru_sk_s {
  // Associated public key
  mam_ntru_pk_t public_key;
  // Secret key - small coefficients of polynomial f
  trit_t secret_key[MAM_NTRU_SK_SIZE];
  // Internal representation of a private key: NTT(1+3f)
  poly_t f;
} mam_ntru_sk_t;

typedef struct mam_ntru_sk_t_set_entry_s mam_ntru_sk_t_set_entry_t;
typedef mam_ntru_sk_t_set_entry_t* mam_ntru_sk_t_set_t;

#endif  // __MAM_NTRU_NTRU_TYPES_H__

/** @} */
