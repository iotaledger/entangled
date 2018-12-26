/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROB_MAP_H__
#define __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROB_MAP_H__

#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
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

void iota_consensus_exit_prob_map_reset(
    ep_randomizer_t *const exit_probability_randomizer);

void iota_consensus_exit_prob_map_init(ep_randomizer_t *const randomizer);

retcode_t iota_consensus_exit_prob_map_randomize(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const ep_validator,
    cw_calc_result *const cw_result, flex_trit_t const *const ep,
    flex_trit_t *tip);
/**
 * Calculates exit and overall transition probabilities
 *
 * @param exit_probability_randomizer The base class
 * @param ep_validator The validator - used for the entry point
 * @param cw_result The cumulative weight data, Null means that cw will be
 * recalculated
 * @param ep The entry point hash
 * @param hash_to_exit_probs The mapping of the calculated exit probabilities -
 * non tips will have 0 probability
 * @param hash_to_trans_probs The mapping of the calculated transition
 * probabilities
 *
 * @return a status code
 */

retcode_t iota_consensus_exit_prob_map_calculate_probs(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const ep_validator,
    cw_calc_result *const cw_result, flex_trit_t const *const ep,
    hash_to_double_map_t *const hash_to_exit_probs,
    hash_to_double_map_t *const hash_to_trans_probs);

/**
 * Virtual distructor
 *
 * @param exit_probability_randomizer The base class
 *
 * @return void
 */

void iota_consensus_exit_prob_map_destroy(
    ep_randomizer_t *const exit_probability_randomizer);

/**
 * Extracts tips into set
 *
 * @param cw_result The cumulative weight data
 * @param tips The result set
 *
 * @return void
 */

void iota_consensus_exit_prob_map_eps_extract_tips(
    cw_calc_result *const cw_result, hash243_set_t *const tips);

double iota_consensus_exit_prob_map_sum_probs(
    hash_to_double_map_t *const hash_to_probs);

static ep_randomizer_vtable exit_prob_map_vtable = {
    .exit_probability_randomize = iota_consensus_exit_prob_map_randomize,
    .exit_probability_destroy = iota_consensus_exit_prob_map_destroy,
};

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROB_MAP_H__
