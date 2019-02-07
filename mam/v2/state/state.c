/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/state/state.h"
#include "mam/v2/mam/mam_pre_shared_key_t_set.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/prng/prng.h"

retcode_t mam_state_init(mam_state_t* const state,
                         trits_t const prng_secret_key) {
  retcode_t ret = RC_OK;

  if ((ret = mam_prng_init(&state->prng, prng_secret_key)) != RC_OK) {
    return ret;
  }
  state->ntru_sks = NULL;
  state->psks = NULL;

  return ret;
}

retcode_t mam_state_destroy(mam_state_t* const state) {
  mam_prng_destroy(&state->prng);
  mam_ntru_sk_t_set_free(&state->ntru_sks);
  mam_pre_shared_key_t_set_free(&state->psks);

  return RC_OK;
}

retcode_t mam_state_add_ntru_sk(mam_state_t* const state,
                                mam_ntru_sk_t const* const ntru_sk) {
  return mam_ntru_sk_t_set_add(&state->ntru_sks, ntru_sk);
}
