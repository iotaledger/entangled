/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_GLOBAL_CALCS_H__
#define __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_GLOBAL_CALCS_H__

#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"
#include "common/errors.h"
#include "utils/containers/hash/hash_int64_t_map.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maps transition probabilities to direct approvers
 *
 * @param alpha The alpha param of the random walk
 * @param cw_ratings The cumulative weights data
 * @param approvers The set of direct approvers
 * @param transition_probs The result is in the order of iteration over
 * "approvers"
 *
 * @return a status code
 */

extern retcode_t map_transition_probabilities(double alpha, hash_to_int64_t_map_t const cw_ratings,
                                              hash243_set_t const *const approvers, double transition_probs[]);

static inline bool iota_consensus_is_tx_a_tip(hash_to_indexed_hash_set_map_t const *const tx_to_approvers,
                                              flex_trit_t const *const tx) {
  hash_to_indexed_hash_set_entry_t *aps = NULL;
  hash_to_indexed_hash_set_map_find(tx_to_approvers, tx, &aps);
  return hash243_set_size(&aps->approvers) == 0;
}
#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_GLOBAL_CALCS_H__
