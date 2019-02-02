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

retcode_t mam_state_init(mam_state_t* const state) {
  state->psks = NULL;

  return RC_OK;
}

retcode_t mam_state_destroy(mam_state_t* const state) {
  mam_pre_shared_key_t_set_free(&state->psks);

  return RC_OK;
}
