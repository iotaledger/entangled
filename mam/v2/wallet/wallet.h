/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_WALLET_WALLET_H__
#define __MAM_V2_WALLET_WALLET_H__

#include "common/errors.h"
#include "mam/v2/mam/mam_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_wallet_s {
  mam_pre_shared_key_t_set_t psks;
} mam_wallet_t;

retcode_t mam_wallet_init(mam_wallet_t* const wallet);
retcode_t mam_wallet_destroy(mam_wallet_t* const wallet);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_WALLET_WALLET_H__
