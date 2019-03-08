/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_PSK_PSK_H__
#define __MAM_PSK_PSK_H__

#include "common/errors.h"
#include "mam/trits/trits.h"

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
trits_t mam_psk_key(mam_psk_t const* const psk);

void mam_psk_destroy(mam_psk_t* const psk);
void mam_psks_destroy(mam_psk_t_set_t* const psks);

size_t mam_psks_serialized_size(mam_psk_t_set_t const psks);
retcode_t mam_psks_serialize(mam_psk_t_set_t const psks, trits_t* const trits);
retcode_t mam_psks_deserialize(trits_t* const trits,
                               mam_psk_t_set_t* const psks);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_PSK_PSK_H__
