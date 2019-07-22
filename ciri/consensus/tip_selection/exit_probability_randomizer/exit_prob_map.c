/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <math.h>

#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_prob_map.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/global_calcs.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define EXIT_PROB_MAP_LOGGER_ID "exit_prob_map"

static logger_id_t logger_id;

/*
 * Private functions
 */

static void iota_consensus_exit_prob_map_add_exit_probs(hash_to_double_map_t *const *const hash_to_probs,
                                                        flex_trit_t const *const hash, double value) {
  hash_to_double_map_entry_t *curr_approver_entry = NULL;
  if (hash_to_double_map_find(**hash_to_probs, hash, &curr_approver_entry)) {
    curr_approver_entry->value += value;
  } else {
    hash_to_double_map_add(*hash_to_probs, hash, value);
  }
}

static retcode_t iota_consensus_exit_prob_remove_invalid_tip_candidates(
    tangle_t *const tangle, cw_calc_result *const cw_result, exit_prob_transaction_validator_t *const ep_validator) {
  retcode_t ret = RC_OK;
  hash243_set_entry_t *tip_entry = NULL;
  hash243_set_entry_t *tip_tmp_entry = NULL;
  hash243_set_t tips = NULL;
  hash_to_indexed_hash_set_entry_t *approvers_entry = NULL;
  iota_consensus_exit_prob_map_eps_extract_tips(cw_result, &tips);
  HASH_ITER(hh, tips, tip_entry, tip_tmp_entry) {
    bool has_valid_tail = true;
    if ((ret = iota_consensus_exit_prob_transaction_validator_is_valid(ep_validator, tangle, tip_entry->hash,
                                                                       &has_valid_tail, false)) != RC_OK) {
      log_error(logger_id, "Tail transaction validation failed: %" PRIu64 "\n", ret);
      goto done;
    }
    if (!has_valid_tail) {
      if (!hash_to_indexed_hash_set_map_find(&cw_result->tx_to_approvers, tip_entry->hash, &approvers_entry)) {
        goto done;
      }
      hash243_set_remove(&approvers_entry->approvers, tip_entry->hash);
    }
  }

done:
  hash243_set_free(&tips);
  return ret;
}

/*
 * Public functions
 */

void iota_consensus_exit_prob_map_init(ep_randomizer_t *const randomizer) {
  randomizer->base.vtable = exit_prob_map_vtable;
  ep_prob_map_randomizer_t *prob_randomizer = (ep_prob_map_randomizer_t *const)randomizer;
  prob_randomizer->exit_probs = NULL;
  prob_randomizer->transition_probs = NULL;
  logger_id = logger_helper_enable(EXIT_PROB_MAP_LOGGER_ID, LOGGER_DEBUG, true);
}

retcode_t iota_consensus_exit_prob_map_randomize(ep_randomizer_t const *const randomizer, tangle_t *const tangle,
                                                 exit_prob_transaction_validator_t *const ep_validator,
                                                 cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                 flex_trit_t *tip) {
  retcode_t ret;
  ep_prob_map_randomizer_t *prob_randomizer = (ep_prob_map_randomizer_t *)randomizer;

  if (prob_randomizer->exit_probs == NULL) {
    if ((ret = iota_consensus_exit_prob_map_calculate_probs(randomizer, tangle, ep_validator, cw_result, ep,
                                                            &prob_randomizer->exit_probs,
                                                            &prob_randomizer->transition_probs)) != RC_OK) {
      return ret;
    }
  }

  hash243_set_t tips = NULL;
  iota_consensus_exit_prob_map_eps_extract_tips(cw_result, &tips);
  // Randomizes uniformly a value between 0 to 1
  double rand_weight = rand_handle_probability();
  hash243_set_entry_t *tip_entry = NULL;
  hash243_set_entry_t *tip_tmp_entry = NULL;
  hash_to_double_map_entry_t *curr_ep = NULL;

  HASH_ITER(hh, tips, tip_entry, tip_tmp_entry) {
    hash_to_double_map_find(prob_randomizer->exit_probs, tip_entry->hash, &curr_ep);
    rand_weight -= curr_ep->value;
    if (rand_weight <= 0) {
      memcpy(tip, tip_entry->hash, FLEX_TRIT_SIZE_243);
      break;
    }
  }
  hash243_set_free(&tips);
  return RC_OK;
}

retcode_t iota_consensus_exit_prob_map_calculate_probs(ep_randomizer_t const *const exit_probability_randomizer,
                                                       tangle_t *const tangle,
                                                       exit_prob_transaction_validator_t *const ep_validator,
                                                       cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                       hash_to_double_map_t *const hash_to_exit_probs,
                                                       hash_to_double_map_t *const hash_to_trans_probs) {
  retcode_t ret;
  bool ep_is_valid = false;
  hash243_queue_t queue = NULL;
  hash243_queue_entry_t *curr_tx = NULL;
  hash_to_indexed_hash_set_entry_t *aps = NULL;
  hash_to_double_map_entry_t *curr_tx_entry = NULL;
  hash243_set_entry_t *approver_entry = NULL;
  hash243_set_entry_t *tmp_entry = NULL;
  size_t num_approvers;
  size_t approver_idx;

  if ((ret = iota_consensus_exit_prob_transaction_validator_is_valid(ep_validator, tangle, ep, &ep_is_valid, true)) !=
      RC_OK) {
    log_error(logger_id, "Entry point validation failed: %" PRIu64 "\n", ret);
    return ret;
  } else if (!ep_is_valid) {
    log_error(logger_id, "Invalid entry point\n");
    return RC_EXIT_PROBABILITIES_INVALID_ENTRYPOINT;
  }

  if ((ret = iota_consensus_exit_prob_remove_invalid_tip_candidates(tangle, cw_result, ep_validator)) != RC_OK) {
    return ret;
  }

  hash_to_double_map_add(hash_to_trans_probs, ep, 1);
  hash_to_double_map_add(hash_to_exit_probs, ep, 1);
  hash243_queue_push(&queue, ep);

  while (hash243_queue_count(queue)) {
    curr_tx = hash243_queue_pop(&queue);
    hash_to_indexed_hash_set_map_find(&cw_result->tx_to_approvers, curr_tx->hash, &aps);
    hash_to_double_map_find(*hash_to_exit_probs, curr_tx->hash, &curr_tx_entry);
    num_approvers = hash243_set_size(aps->approvers);
    double trans_probs_to_direct_approvers[num_approvers];
    map_transition_probabilities(exit_probability_randomizer->conf->alpha, cw_result->cw_ratings, &aps->approvers,
                                 trans_probs_to_direct_approvers);
    approver_idx = 0;
    HASH_ITER(hh, aps->approvers, approver_entry, tmp_entry) {
      iota_consensus_exit_prob_map_add_exit_probs(
          &hash_to_exit_probs, approver_entry->hash,
          (trans_probs_to_direct_approvers[approver_idx] * curr_tx_entry->value));
      iota_consensus_exit_prob_map_add_exit_probs(
          &hash_to_trans_probs, approver_entry->hash,
          (trans_probs_to_direct_approvers[approver_idx] * curr_tx_entry->value));
      hash243_queue_push(&queue, approver_entry->hash);
      approver_idx++;
    }

    if (!iota_consensus_is_tx_a_tip(&cw_result->tx_to_approvers, curr_tx->hash)) {
      curr_tx_entry->value = 0;
    }
    free(curr_tx);
  }

  hash243_queue_free(&queue);

  return RC_OK;
}

void iota_consensus_exit_prob_map_eps_extract_tips(cw_calc_result *const cw_result, hash243_set_t *const tips) {
  hash_to_indexed_hash_set_entry_t *curr_hash_to_approvers_entry = NULL;
  hash_to_indexed_hash_set_entry_t *tmp_hash_to_approvers_entry = NULL;
  HASH_ITER(hh, cw_result->tx_to_approvers, curr_hash_to_approvers_entry, tmp_hash_to_approvers_entry) {
    if (iota_consensus_is_tx_a_tip(&cw_result->tx_to_approvers, curr_hash_to_approvers_entry->hash)) {
      hash243_set_add(tips, curr_hash_to_approvers_entry->hash);
    }
  }
}

double iota_consensus_exit_prob_map_sum_probs(hash_to_double_map_t *const hash_to_probs) {
  double sum = 0;
  hash_to_double_map_entry_t *exit_prob_entry = NULL;
  hash243_set_entry_t *tip_entry = NULL;
  hash243_set_entry_t *tmp_tip_entry = NULL;
  hash243_set_t keys = NULL;
  hash_to_double_map_keys(hash_to_probs, &keys);
  HASH_ITER(hh, keys, tip_entry, tmp_tip_entry) {
    if (hash_to_double_map_find(*hash_to_probs, tip_entry->hash, &exit_prob_entry)) {
      sum += exit_prob_entry->value;
    }
  }
  hash243_set_free(&keys);
  return sum;
}

retcode_t iota_consensus_exit_prob_map_destroy(ep_randomizer_t *const exit_probability_randomizer) {
  logger_helper_release(logger_id);
  return iota_consensus_exit_prob_map_reset(exit_probability_randomizer);
}

retcode_t iota_consensus_exit_prob_map_reset(ep_randomizer_t *const exit_probability_randomizer) {
  ep_prob_map_randomizer_t *prob_randomizer = (ep_prob_map_randomizer_t *const)exit_probability_randomizer;
  if (prob_randomizer->exit_probs) {
    hash_to_double_map_free(&prob_randomizer->exit_probs);
    hash_to_double_map_free(&prob_randomizer->transition_probs);
  }
  return RC_OK;
}
