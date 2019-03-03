/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/troika/troika.h"
#include "mam/trits/trits.h"

void mam_ftroika_transform(trit_t *const state, size_t state_size) {
  size_t i;
  t27_t fstate[SLICES];
  for (i = 0; i != state_size; ++i) {
    state[i] += 1;
  }
  state_to_fstate(state, fstate);
  ftroika_permutation(fstate, MAM_TROIKA_NUM_ROUNDS);
  fstate_to_state(fstate, state);
  for (i = 0; i != state_size; ++i) {
    state[i] -= 1;
  }
}

void mam_troika_transform(trit_t *const state, size_t state_size) {
  size_t i;
  for (i = 0; i != state_size; ++i) {
    state[i] += 1;
  }
  troika_permutation(state, MAM_TROIKA_NUM_ROUNDS);
  for (i = 0; i != state_size; ++i) {
    state[i] -= 1;
  }
}
