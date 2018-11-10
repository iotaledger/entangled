/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__
#define __COMMON_CONCENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/storage/connection.h"
#include "consensus/conf.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/tangle/tangle.h"
#include "utils/hash_maps.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Should align with IRI's walker validator, we name it differently
/// Because it can validate txs when doing random walk or when calculating
/// exit probabilities

typedef struct exit_prob_transaction_validator_s {
  iota_consensus_conf_t *conf;
  tangle_t *tangle;
  milestone_tracker_t *mt;
  ledger_validator_t *lv;
  state_delta_t delta;
  hash243_set_t analyzed_hashes;
  hash243_set_t max_depth_ok_memoization;
} exit_prob_transaction_validator_t;

extern retcode_t iota_consensus_exit_prob_transaction_validator_init(
    iota_consensus_conf_t *const conf, tangle_t *const tangle,
    milestone_tracker_t *const mt, ledger_validator_t *const lv,
    exit_prob_transaction_validator_t *epv);

extern retcode_t iota_consensus_exit_prob_transaction_validator_destroy(
    exit_prob_transaction_validator_t *epv);

extern retcode_t iota_consensus_exit_prob_transaction_validator_is_valid(
    exit_prob_transaction_validator_t *const epv,
    trit_array_t const *const tail_hash, bool *const is_valid);

extern retcode_t iota_consensus_exit_prob_transaction_validator_below_max_depth(
    exit_prob_transaction_validator_t *ep_validator,
    trit_array_t const *const tail_hash, uint32_t lowest_allowed_depth,
    bool *below_max_depth);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__
