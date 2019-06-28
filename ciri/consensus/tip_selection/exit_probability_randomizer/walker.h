/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__
#define __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__

#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_probability_randomizer.h"
#include "ciri/consensus/tip_selection/exit_probability_validator/exit_probability_validator.h"
#include "common/errors.h"
#include "utils/containers/hash/hash_double_map.h"

#ifdef __cplusplus
extern "C" {
#endif

void iota_consensus_random_walker_init(ep_randomizer_t *const randomizer);

retcode_t iota_consensus_random_walker_randomize(ep_randomizer_t const *const exit_probability_randomizer,
                                                 tangle_t *const tangle,
                                                 exit_prob_transaction_validator_t *const ep_validator,
                                                 cw_calc_result *const cw_result, flex_trit_t const *const ep,
                                                 flex_trit_t *tip);

static ep_randomizer_vtable random_walk_vtable = {
    .exit_probability_randomize = iota_consensus_random_walker_randomize,
    .exit_probability_destroy = NULL,
};

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__
