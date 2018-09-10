/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONCENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__
#define __CONCENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"

#include "common/storage/connection.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t iota_consensus_random_walker_randomize(
    const ep_randomizer_t *exit_probability_randomizer,
    const exit_prob_transaction_validator_t *ep_validator,
    cw_calc_result *cw_result, const trit_array_p ep, trit_array_p tip);

static ep_randomizer_vtable random_walk_vtable = {
    .exit_probability_randomize = iota_consensus_random_walker_randomize,
};

void init_ep_randomizer_walker(ep_randomizer_t *randomizer);

#ifdef __cplusplus
}
#endif

#endif  // __CONCENSUS_EXIT_PROBABILITY_RANDOMIZER_WALKER_H__
