/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_randomizer/walker.h"
#include "utils/logger_helper.h"

#define EXIT_PROBABILITY_RANDOMIZER_LOGGER_ID \
  "consensus_exit_probability_randomizer"

retcode_t iota_consensus_ep_randomizer_init(
    ep_randomizer_t *const ep_randomizer, tangle_t *const tangle, double alpha,
    enum ep_randomizer_implementation impl) {
  logger_helper_init(EXIT_PROBABILITY_RANDOMIZER_LOGGER_ID, LOGGER_DEBUG, true);
  srand(time(NULL));
  ep_randomizer->tangle = tangle;
  ep_randomizer->alpha = alpha;
  if (impl == EP_RANDOM_WALK) {
    iota_consensus_random_walker_init(ep_randomizer);
  } else if (impl == EP_NO_IMPLEMENTATION) {
    return RC_CONSENSUS_NOT_IMPLEMENTED;
  }
  return RC_OK;
}

retcode_t iota_consensus_ep_randomizer_destroy(
    ep_randomizer_t *const ep_randomizer) {
  logger_helper_destroy(EXIT_PROBABILITY_RANDOMIZER_LOGGER_ID);
  ep_randomizer->tangle = NULL;
  return RC_OK;
}

retcode_t iota_consensus_exit_probability_randomize(
    ep_randomizer_t const *const ep_randomizer,
    exit_prob_transaction_validator_t const *const epv,
    cw_calc_result *const cw_result, trit_array_t const *const ep,
    trit_array_t *const tip) {
  return ep_randomizer->base.vtable.exit_probability_randomize(
      ep_randomizer, epv, cw_result, ep, tip);
}
