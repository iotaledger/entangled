/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROBABILITY_RANDOMIZER_H__
#define __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROBABILITY_RANDOMIZER_H__

#include <stdint.h>

#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ep_randomizer_implementation {
  EP_NO_IMPLEMENTATION,
  EP_RANDOM_WALK,
  EP_RANDOMIZE_SAMPLE,
};

typedef struct ep_randomizer_base_s ep_randomizer_base_t;
typedef struct ep_randomizer_s ep_randomizer_t;
typedef struct {
  // find_transactions_request
  retcode_t (*exit_probability_randomize)(
      const ep_randomizer_t *ep_randomizer,
      exit_prob_transaction_validator_t *epv, cw_calc_result *cw_result,
      const trit_array_p ep, trit_array_p tip);
} ep_randomizer_vtable;

struct ep_randomizer_base_s {
  ep_randomizer_vtable vtable;
};

struct ep_randomizer_s {
  ep_randomizer_base_t base;
  tangle_t *tangle;
  double alpha;
};

extern retcode_t iota_consensus_ep_randomizer_init(
    tangle_t *const tangle, ep_randomizer_t *ep_randomizer, double alpha,
    enum ep_randomizer_implementation);
extern retcode_t iota_consensus_ep_randomizer_destroy(
    ep_randomizer_t *ep_randomizer);

extern retcode_t iota_consensus_exit_probability_randomize(
    const ep_randomizer_t *exit_probability_randomizer,
    exit_prob_transaction_validator_t *ep_validator, cw_calc_result *cw_result,
    const trit_array_p ep, trit_array_p tip);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROBABILITY_RANDOMIZER_H__
