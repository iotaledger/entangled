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
\file mam.h
\brief MAM2 layer.
*/
#ifndef __MAM_V2_MAM_MAM_TYPES_H__
#define __MAM_V2_MAM_MAM_TYPES_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAM2_KEY_ID_SIZE 81
#define MAM2_PSK_ID_SIZE 81
#define MAM2_PSK_SIZE 243

// TODO - move to common def file
/*! \brief NTRU public key - 3g(x)/(1+3f(x)) - size. */
#define MAM2_NTRU_PK_SIZE 9216

/*! \brief Preshared key. */
typedef struct mam_pre_shared_key_s {
  trit_t id[MAM2_PSK_ID_SIZE];
  trit_t pre_shared_key[MAM2_PSK_SIZE];
} mam_pre_shared_key_t;

/*! \brief Recipient's NTRU public key. */
typedef struct mam_ntru_pk_s {
  trit_t pk[MAM2_NTRU_PK_SIZE];
} mam_ntru_pk_t;

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MAM_TYPES_H__
