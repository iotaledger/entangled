/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_probability_randomizer.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_prob_map.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/walker.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"

#define EXIT_PROBABILITY_RANDOMIZER_LOGGER_ID "exit_probability_randomizer"

static logger_id_t logger_id;

retcode_t iota_consensus_ep_randomizer_init(ep_randomizer_t *const ep_randomizer, iota_consensus_conf_t *const conf,
                                            ep_randomizer_implementation_t impl) {
  logger_id = logger_helper_enable(EXIT_PROBABILITY_RANDOMIZER_LOGGER_ID, LOGGER_DEBUG, true);
  rand_handle_seed(time(NULL));
  ep_randomizer->conf = conf;
  if (impl == EP_RANDOM_WALK) {
    iota_consensus_random_walker_init(ep_randomizer);
  } else if (impl == EP_RANDOMIZE_MAP_AND_SAMPLE) {
    iota_consensus_exit_prob_map_init(ep_randomizer);
  } else if (impl == EP_NO_IMPLEMENTATION) {
    return RC_EXIT_PROBABILITIES_NOT_IMPLEMENTED;
  }
  return RC_OK;
}

retcode_t iota_consensus_ep_randomizer_destroy(ep_randomizer_t *const ep_randomizer) {
  if (ep_randomizer->base.vtable.exit_probability_destroy != NULL) {
    ep_randomizer->base.vtable.exit_probability_destroy(ep_randomizer);
  }
  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_consensus_exit_probability_randomize(ep_randomizer_t const *const ep_randomizer, tangle_t *const tangle,
                                                    exit_prob_transaction_validator_t *const epv,
                                                    cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                    flex_trit_t *tip) {
  return ep_randomizer->base.vtable.exit_probability_randomize(ep_randomizer, tangle, epv, cw_result, ep, tip);
}
