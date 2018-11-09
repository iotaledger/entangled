/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <math.h>

#include "consensus/exit_probability_randomizer/walker.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define RANDOM_WALKER_LOGGER_ID "consensus_random_walker"

/*
 * Private functions
 */

static retcode_t select_approver(
    ep_randomizer_t const *const exit_probability_randomizer,
    hash_int_map_t const cw_ratings, hash243_set_t const *const approvers,
    trit_array_t *const approver) {
  hash243_set_entry_t *curr_approver = NULL;
  hash243_set_entry_t *tmp_approver = NULL;
  hash_to_int_map_entry_t *curr_rating = NULL;
  size_t num_approvers = HASH_COUNT(*approvers);
  int64_t weights[num_approvers];
  double sum_weights = 0;
  double target = 0;
  int64_t max_weight = 0;
  size_t idx = 0;

  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    HASH_FIND(hh, cw_ratings, curr_approver->hash, FLEX_TRIT_SIZE_243,
              curr_rating);
    if (curr_rating == NULL) {
      log_error(RANDOM_WALKER_LOGGER_ID, "No rating found for approver\n");
      return RC_CONSENSUS_EXIT_PROBABILITIES_MISSING_RATING;
    }
    weights[idx++] = curr_rating->value;
    max_weight = MAX(max_weight, curr_rating->value);
  }

  for (idx = 0; idx < num_approvers; ++idx) {
    weights[idx] -= max_weight;
    weights[idx] = exp(weights[idx] * exit_probability_randomizer->conf->alpha);
    sum_weights += weights[idx];
  }

  idx = 0;
  target = rand_handle_probability() * sum_weights;
  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    if ((target = (target - weights[idx++])) <= 0) {
      memcpy(approver->trits, curr_approver->hash, FLEX_TRIT_SIZE_243);
      break;
    }
  }

  return RC_OK;
}

static retcode_t find_tail_if_valid(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const epv, trit_array_t *const tx_hash,
    bool *const has_valid_tail) {
  retcode_t ret = RC_OK;

  *has_valid_tail = false;
  ret = iota_tangle_find_tail(exit_probability_randomizer->tangle, tx_hash,
                              tx_hash, has_valid_tail);
  if (ret != RC_OK) {
    log_error(RANDOM_WALKER_LOGGER_ID, "Finding tail failed: %" PRIu64 "\n",
              ret);
    return ret;
  }
  if (*has_valid_tail) {
    ret = iota_consensus_exit_prob_transaction_validator_is_valid(
        epv, tx_hash, has_valid_tail);
    if (ret != RC_OK) {
      log_error(RANDOM_WALKER_LOGGER_ID,
                "Tail transaction validation failed: %" PRIu64 "\n", ret);
      return ret;
    }
  }
  return ret;
}

static retcode_t random_walker_select_approver_tail(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const epv,
    cw_calc_result *const cw_result, flex_trit_t const *const curr_tail_hash,
    trit_array_t *const approver, bool *const has_approver_tail) {
  retcode_t ret = RC_OK;
  hash_to_indexed_hash_set_entry_t *approvers_entry = NULL;
  hash243_set_entry_t *approver_entry = NULL;

  *has_approver_tail = false;

  HASH_FIND(hh, cw_result->tx_to_approvers, curr_tail_hash, FLEX_TRIT_SIZE_243,
            approvers_entry);
  if (!approvers_entry) {
    return RC_OK;
  }

  while (!(*has_approver_tail) && HASH_COUNT(approvers_entry->approvers) > 0) {
    ret = select_approver(exit_probability_randomizer, cw_result->cw_ratings,
                          &approvers_entry->approvers, approver);
    if (ret != RC_OK) {
      *has_approver_tail = false;
      return ret;
    }
    HASH_FIND(hh, approvers_entry->approvers, approver->trits,
              FLEX_TRIT_SIZE_243, approver_entry);
    ret = find_tail_if_valid(exit_probability_randomizer, epv, approver,
                             has_approver_tail);
    if (ret != RC_OK) {
      *has_approver_tail = false;
      return ret;
    }
    if (!(*has_approver_tail)) {
      // if next tail is not valid, re-select while removing it from
      // approvers set
      HASH_DEL(approvers_entry->approvers, approver_entry);
    }
  }
  return ret;
}

/*
 * Public functions
 */

void iota_consensus_random_walker_init(ep_randomizer_t *const randomizer) {
  randomizer->base.vtable = random_walk_vtable;
}

retcode_t iota_consensus_random_walker_randomize(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const ep_validator,
    cw_calc_result *const cw_result, trit_array_t const *const ep,
    trit_array_t *const tip) {
  retcode_t ret = RC_OK;
  bool ep_is_valid = false;
  bool has_approver_tail = false;
  size_t num_traversed_tails = 1;
  flex_trit_t *curr_tail_hash = ep->trits;
  flex_trit_t approver_tail_hash[FLEX_TRIT_SIZE_243];
  trit_array_t approver_tail = {.trits = approver_tail_hash,
                                .num_trits = HASH_LENGTH_TRIT,
                                .num_bytes = FLEX_TRIT_SIZE_243,
                                .dynamic = 0};

  ret = iota_consensus_exit_prob_transaction_validator_is_valid(
      ep_validator, ep, &ep_is_valid);
  if (ret != RC_OK) {
    log_error(RANDOM_WALKER_LOGGER_ID,
              "Entry point validation failed: %" PRIu64 "\n", ret);
    return ret;
  } else if (!ep_is_valid) {
    log_error(RANDOM_WALKER_LOGGER_ID, "Invalid entry point\n");
    return RC_CONSENSUS_EXIT_PROBABILITIES_INVALID_ENTRYPOINT;
  }

  do {
    ret = random_walker_select_approver_tail(
        exit_probability_randomizer, ep_validator, cw_result, curr_tail_hash,
        &approver_tail, &has_approver_tail);
    if (ret != RC_OK) {
      log_error(RANDOM_WALKER_LOGGER_ID,
                "Selecting approver tail failed: %" PRIu64 "\n", ret);
      return ret;
    } else if (has_approver_tail) {
      curr_tail_hash = approver_tail_hash;
      num_traversed_tails++;
    }
  } while (has_approver_tail);

  memcpy(tip->trits, curr_tail_hash, FLEX_TRIT_SIZE_243);
  log_debug(RANDOM_WALKER_LOGGER_ID,
            "Number of tails traversed to find tip: %" PRIu64 "\n",
            num_traversed_tails);

  return ret;
}
