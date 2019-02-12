/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_STATE_STATE_H__
#define __MAM_V2_STATE_STATE_H__

#include "common/errors.h"
#include "mam/v2/mam/mam_types.h"
#include "mam/v2/ntru/ntru_types.h"
#include "mam/v2/prng/prng_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_state_s {
  mam_prng_t prng;
  mam_ntru_sk_t_set_t ntru_sks;
  mam_ntru_pk_t_set_t ntru_pks;
  mam_psk_t_set_t psks;
} mam_state_t;

retcode_t mam_state_init(mam_state_t* const state,
                         trits_t const prng_secret_key);
retcode_t mam_state_destroy(mam_state_t* const state);

retcode_t mam_state_add_ntru_sk(mam_state_t* const state,
                                mam_ntru_sk_t const* const ntru_sk);

retcode_t mam_state_add_ntru_pk(mam_state_t* const state,
                                mam_ntru_pk_t const* const ntru_pk);

retcode_t mam_state_add_psk(mam_state_t* const state,
                            mam_psk_t const* const psk);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_STATE_STATE_H__
