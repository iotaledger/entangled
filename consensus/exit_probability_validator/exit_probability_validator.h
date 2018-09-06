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
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone/milestone.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Should align with IRI's walker validator, we name it differently
/// Because it can validate txs when doing random walk or when calculating
/// exit probabilities

typedef struct exit_prob_transaction_validator_s {
  tangle_t *tangle;
  milestone_t *milestone;
  ledger_validator_t *lv;
  // private final TransactionValidator transactionValidator;
} exit_prob_transaction_validator_t;

extern retcode_t iota_consensus_exit_prob_transaction_validator_init(
    const tangle_t *tangle, const milestone_t *milestone,
    const ledger_validator_t *lv, exit_prob_transaction_validator_t *epv);

extern retcode_t iota_consensus_exit_prob_transaction_validator_destroy(
    exit_prob_transaction_validator_t *epv);

extern retcode_t iota_consensus_exit_prob_transaction_validator_is_valid(
    const exit_prob_transaction_validator_t *ep_validator, trit_array_p tx_hash,
    bool *is_valid);

extern retcode_t iota_consensus_exit_prob_transaction_validator_below_max_depth(
    const exit_prob_transaction_validator_t *ep_validator, trit_array_p tip,
    size_t depth);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__
