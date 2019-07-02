/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <math.h>

#include "ciri/consensus/tip_selection/exit_probability_randomizer/global_calcs.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/walker.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define RANDOM_WALKER_LOGGER_ID "random_walker"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t select_approver(ep_randomizer_t const *const exit_probability_randomizer,
                                 hash_to_int64_t_map_t const cw_ratings, hash243_set_t const *const approvers,
                                 flex_trit_t *const approver) {
  hash243_set_entry_t *curr_approver = NULL;
  hash243_set_entry_t *tmp_approver = NULL;
  size_t num_approvers = hash243_set_size(*approvers);
  double transition_probs[num_approvers];
  double sum_transition_probs = 0;
  double target = 0;
  size_t idx = 0;
  retcode_t ret;

  if ((ret = map_transition_probabilities(exit_probability_randomizer->conf->alpha, cw_ratings, approvers,
                                          transition_probs)) != RC_OK) {
    return ret;
  }

  for (idx = 0; idx < num_approvers; ++idx) {
    sum_transition_probs += transition_probs[idx];
  }

  idx = 0;
  target = rand_handle_probability() * sum_transition_probs;
  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    if ((target = (target - transition_probs[idx++])) <= 0) {
      memcpy(approver, curr_approver->hash, FLEX_TRIT_SIZE_243);
      break;
    }
  }

  return RC_OK;
}

static retcode_t find_tail_if_valid(tangle_t *const tangle, exit_prob_transaction_validator_t *const epv,
                                    flex_trit_t *const tx_hash, bool *const has_valid_tail) {
  retcode_t ret = RC_OK;

  *has_valid_tail = false;
  ret = iota_tangle_find_tail(tangle, tx_hash, tx_hash, has_valid_tail);
  if (ret != RC_OK) {
    log_error(logger_id, "Finding tail failed: %" PRIu64 "\n", ret);
    return ret;
  }
  if (*has_valid_tail) {
    ret = iota_consensus_exit_prob_transaction_validator_is_valid(epv, tangle, tx_hash, has_valid_tail);
    if (ret != RC_OK) {
      log_error(logger_id, "Tail transaction validation failed: %" PRIu64 "\n", ret);
      return ret;
    }
  }
  return ret;
}

static retcode_t random_walker_select_approver_tail(ep_randomizer_t const *const exit_probability_randomizer,
                                                    tangle_t *const tangle,
                                                    exit_prob_transaction_validator_t *const epv,
                                                    cw_calc_result *const cw_result,
                                                    flex_trit_t const *const curr_tail_hash,
                                                    flex_trit_t *const approver, bool *const has_approver_tail) {
  retcode_t ret = RC_OK;
  hash_to_indexed_hash_set_entry_t *approvers_entry = NULL;

  *has_approver_tail = false;
  if (!hash_to_indexed_hash_set_map_find(&cw_result->tx_to_approvers, curr_tail_hash, &approvers_entry)) {
    return RC_OK;
  }

  while (!(*has_approver_tail) && HASH_COUNT(approvers_entry->approvers) > 0) {
    ret = select_approver(exit_probability_randomizer, cw_result->cw_ratings, &approvers_entry->approvers, approver);
    if (ret != RC_OK) {
      *has_approver_tail = false;
      return ret;
    }

    if ((ret = find_tail_if_valid(tangle, epv, approver, has_approver_tail)) != RC_OK) {
      return ret;
    }
    if (!(*has_approver_tail)) {
      // if next tail is not valid, re-select while removing it from
      // approvers set
      hash243_set_remove(&approvers_entry->approvers, approver);
    }
  }
  return ret;
}

/*
 * Public functions
 */

void iota_consensus_random_walker_init(ep_randomizer_t *const randomizer) {
  logger_id = logger_helper_enable(RANDOM_WALKER_LOGGER_ID, LOGGER_DEBUG, true);
  randomizer->base.vtable = random_walk_vtable;
}

retcode_t iota_consensus_random_walker_randomize(ep_randomizer_t const *const exit_probability_randomizer,
                                                 tangle_t *const tangle,
                                                 exit_prob_transaction_validator_t *const ep_validator,
                                                 cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                 flex_trit_t *tip) {
  retcode_t ret = RC_OK;
  bool ep_is_valid = false;
  bool has_approver_tail = false;
  size_t num_traversed_tails = 1;
  flex_trit_t const *curr_tail_hash = ep;
  flex_trit_t approver_tail_hash[FLEX_TRIT_SIZE_243];
  if ((ret = iota_consensus_exit_prob_transaction_validator_is_valid(ep_validator, tangle, ep, &ep_is_valid)) !=
      RC_OK) {
    log_error(logger_id, "Entry point validation failed: %" PRIu64 "\n", ret);
    return ret;
  } else if (!ep_is_valid) {
    log_error(logger_id, "Invalid entry point\n");
    return RC_EXIT_PROBABILITIES_INVALID_ENTRYPOINT;
  }

  do {
    if ((ret = random_walker_select_approver_tail(exit_probability_randomizer, tangle, ep_validator, cw_result,
                                                  curr_tail_hash, approver_tail_hash, &has_approver_tail)) != RC_OK) {
      log_error(logger_id, "Selecting approver tail failed: %" PRIu64 "\n", ret);
      return ret;
    } else if (has_approver_tail) {
      curr_tail_hash = approver_tail_hash;
      num_traversed_tails++;
    }
  } while (has_approver_tail);

  memcpy(tip, curr_tail_hash, FLEX_TRIT_SIZE_243);
  log_debug(logger_id, "Number of tails traversed to find tip: %" PRIu64 "\n", num_traversed_tails);

  return ret;
}
