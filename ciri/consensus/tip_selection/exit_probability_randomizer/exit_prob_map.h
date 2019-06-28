/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROB_MAP_H__
#define __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROB_MAP_H__

#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_probability_randomizer.h"
#include "ciri/consensus/tip_selection/exit_probability_validator/exit_probability_validator.h"
#include "common/errors.h"
#include "utils/containers/hash/hash_double_map.h"
#include "utils/time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ep_prob_map_randomizer_s ep_prob_map_randomizer_t;
struct ep_prob_map_randomizer_s {
  ep_randomizer_t base;
  hash_to_double_map_t exit_probs;
  hash_to_double_map_t transition_probs;
};

/**
 * Resets the exit and transition probabilities maps so it can be recalculated,
 * This function needs to be locked for write
 *
 * @param exit_probability_randomizer The base class
 *
 * @return a status code
 */
retcode_t iota_consensus_exit_prob_map_reset(ep_randomizer_t *const exit_probability_randomizer);

void iota_consensus_exit_prob_map_init(ep_randomizer_t *const randomizer);

/**
 * Randomize a tip selection based on exit probabilities
 * This implementation does not validate the entry point upon
 * every randomization but rather when the entire exit probabilities map
 * is being calculated (when "iota_consensus_exit_prob_map_calculate_probs" is
 * called), when the Tangle is updated with new transaction user should
 * recalculate "cw_result" and make sure to call
 * "iota_consensus_exit_prob_map_reset" before randomizing again, to get a new
 * exit probabilities calculation
 *
 * This function needs to be locked for read
 *
 * @param exit_probability_randomizer The base class
 * @param ep_validator The validator - used for the entry point
 * @param cw_result The cumulative weight data
 * @param ep The entry point hash - this is not required in this implementation
 * @param tip The selected tip hash
 *
 * @return a status code
 */
retcode_t iota_consensus_exit_prob_map_randomize(ep_randomizer_t const *const exit_probability_randomizer,
                                                 tangle_t *const tangle,
                                                 exit_prob_transaction_validator_t *const ep_validator,
                                                 cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                 flex_trit_t *tip);
/**
 * Calculates exit and overall transition probabilities
 *
 * This function needs to be locked for write
 *
 * @param exit_probability_randomizer The base class
 * @param ep_validator The validator - used for the entry point
 * @param cw_result The cumulative weight data
 * @param ep The entry point hash
 * @param hash_to_exit_probs The mapping of the calculated exit probabilities -
 * non tips will have 0 probability
 * @param hash_to_trans_probs The mapping of the calculated transition
 * probabilities
 *
 * @return a status code
 */
retcode_t iota_consensus_exit_prob_map_calculate_probs(ep_randomizer_t const *const exit_probability_randomizer,
                                                       tangle_t *const tangle,
                                                       exit_prob_transaction_validator_t *const ep_validator,
                                                       cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                       hash_to_double_map_t *const hash_to_exit_probs,
                                                       hash_to_double_map_t *const hash_to_trans_probs);

/**
 * Virtual distructor
 *
 * This function needs to be locked for write
 *
 * @param exit_probability_randomizer The base class
 *
 * @return a statuc code
 */
retcode_t iota_consensus_exit_prob_map_destroy(ep_randomizer_t *const exit_probability_randomizer);

/**
 * Extracts tips into set
 *
 * This function needs to be locked for read
 *
 * @param cw_result The cumulative weight data
 * @param tips The result set
 *
 * @return void
 */
void iota_consensus_exit_prob_map_eps_extract_tips(cw_calc_result *const cw_result, hash243_set_t *const tips);

double iota_consensus_exit_prob_map_sum_probs(hash_to_double_map_t *const hash_to_probs);

static ep_randomizer_vtable exit_prob_map_vtable = {
    .exit_probability_randomize = iota_consensus_exit_prob_map_randomize,
    .exit_probability_destroy = iota_consensus_exit_prob_map_destroy,
};

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROB_MAP_H__
