/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_MAM_MAM_TYPES_H__
#define __MAM_V2_MAM_MAM_TYPES_H__

#include "mam/v2/defs.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/trits/trits.h"

#define MAM2_PSK_ID_SIZE 81
#define MAM2_PSK_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

// Preshared key
typedef struct mam_pre_shared_key_s {
  trit_t id[MAM2_PSK_ID_SIZE];
  trit_t pre_shared_key[MAM2_PSK_SIZE];
} mam_pre_shared_key_t;

// Recipient's NTRU public key
typedef struct mam_ntru_pk_s {
  trit_t pk[MAM2_NTRU_PK_SIZE];
} mam_ntru_pk_t;

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MAM_TYPES_H__
