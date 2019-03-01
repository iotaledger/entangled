/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/troika/troika.h"
#include "mam/v2/trits/trits.h"

void mam_troika_transform(trit_t *const state, size_t state_size) {
  size_t i;
  t27_t fstate[SLICES];
  for (i = 0; i != state_size; ++i) {
    state[i] += 1;
  }
  state_to_fstate(state, fstate);
  ftroika_permutation(fstate, MAM2_TROIKA_NUM_ROUNDS);
  fstate_to_state(fstate, state);
  for (i = 0; i != state_size; ++i) {
    state[i] -= 1;
  }
}