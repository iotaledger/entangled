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

#define MAM_PSK_ID_SIZE 81
#define MAM_PSK_KEY_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

// Preshared key
typedef struct mam_psk_s {
  trit_t id[MAM_PSK_ID_SIZE];
  trit_t key[MAM_PSK_KEY_SIZE];
} mam_psk_t;

typedef struct mam_psk_t_set_entry_s mam_psk_t_set_entry_t;
typedef mam_psk_t_set_entry_t* mam_psk_t_set_t;

trits_t mam_psk_id(mam_psk_t const* const psk);
trits_t mam_psk_trits(mam_psk_t const* const psk);

size_t mam_psks_serialized_size(mam_psk_t_set_t const psks);
retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t trits);
retcode_t mam_psks_deserialize(trits_t const trits,
                               mam_psk_t_set_t* const psks);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MAM_TYPES_H__
