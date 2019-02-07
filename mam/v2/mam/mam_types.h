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

#include "common/errors.h"
#include "mam/v2/defs.h"
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

typedef struct mam_pre_shared_key_t_set_entry_s
    mam_pre_shared_key_t_set_entry_t;
typedef mam_pre_shared_key_t_set_entry_t* mam_pre_shared_key_t_set_t;

size_t mam_psks_serialized_size(mam_pre_shared_key_t_set_t const psks);
retcode_t mam_psks_serialize(mam_pre_shared_key_t_set_t const psks,
                             trits_t trits);
retcode_t mam_psks_deserialize(trits_t const trits,
                               mam_pre_shared_key_t_set_t* const psks);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MAM_TYPES_H__
