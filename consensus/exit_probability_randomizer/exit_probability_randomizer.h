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

// Forward declarations
typedef struct ep_randomizer_base_s ep_randomizer_base_t;
typedef struct ep_randomizer_s ep_randomizer_t;

typedef enum ep_randomizer_implementation_e {
  EP_NO_IMPLEMENTATION,
  EP_RANDOM_WALK,
  EP_RANDOMIZE_SAMPLE,
} ep_randomizer_implementation_t;

typedef struct {
  // find_transactions_request
  retcode_t (*exit_probability_randomize)(
      ep_randomizer_t const *const ep_randomizer,
      exit_prob_transaction_validator_t *const epv,
      cw_calc_result *const cw_result, trit_array_t const *const ep,
      trit_array_t *const tip);
} ep_randomizer_vtable;

struct ep_randomizer_base_s {
  ep_randomizer_vtable vtable;
};

struct ep_randomizer_s {
  ep_randomizer_base_t base;
  iota_consensus_conf_t *conf;
  tangle_t *tangle;
};

extern retcode_t iota_consensus_ep_randomizer_init(
    ep_randomizer_t *const ep_randomizer, iota_consensus_conf_t *const conf,
    tangle_t *const tangle, ep_randomizer_implementation_t);
extern retcode_t iota_consensus_ep_randomizer_destroy(
    ep_randomizer_t *const ep_randomizer);

extern retcode_t iota_consensus_exit_probability_randomize(
    ep_randomizer_t const *const exit_probability_randomizer,
    exit_prob_transaction_validator_t *const ep_validator,
    cw_calc_result *const cw_result, trit_array_t const *const ep,
    trit_array_t *const tip);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_EXIT_PROBABILITY_RANDOMIZER_EXIT_PROBABILITY_RANDOMIZER_H__
