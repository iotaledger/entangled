/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__
#define __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__

#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"

#ifdef __cplusplus
extern "C" {
#endif

void iota_consensus_random_walker_init(ep_randomizer_t *const randomizer);

retcode_t iota_consensus_random_walker_randomize(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const ep_validator,
    cw_calc_result *const cw_result, trit_array_t const *const ep,
    trit_array_p tip);

static ep_randomizer_vtable random_walk_vtable = {
    .exit_probability_randomize = iota_consensus_random_walker_randomize,
};

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__
