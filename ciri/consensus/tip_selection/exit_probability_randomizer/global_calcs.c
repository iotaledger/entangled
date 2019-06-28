/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>

#include "ciri/consensus/tip_selection/exit_probability_randomizer/global_calcs.h"
#include "utils/macros.h"

retcode_t map_transition_probabilities(double alpha, hash_to_int64_t_map_t const cw_ratings,
                                       hash243_set_t const *const approvers, double transition_probs[]) {
  hash243_set_entry_t *curr_approver = NULL;
  hash243_set_entry_t *tmp_approver = NULL;
  hash_to_int64_t_map_entry_t *curr_rating = NULL;
  size_t num_approvers = hash243_set_size(approvers);
  double sum_transition_probabilities = 0;
  double max_weight = 0;
  size_t idx = 0;

  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    if (!hash_to_int64_t_map_find(&cw_ratings, curr_approver->hash, &curr_rating)) {
      return RC_EXIT_PROBABILITIES_MISSING_RATING;
    }
    transition_probs[idx++] = curr_rating->value;
    max_weight = MAX(max_weight, curr_rating->value);
  }
  for (idx = 0; idx < num_approvers; ++idx) {
    transition_probs[idx] -= max_weight;
    transition_probs[idx] = exp(transition_probs[idx] * alpha);
    sum_transition_probabilities += transition_probs[idx];
  }

  for (idx = 0; idx < num_approvers; ++idx) {
    transition_probs[idx] /= sum_transition_probabilities;
  }

  return RC_OK;
}
