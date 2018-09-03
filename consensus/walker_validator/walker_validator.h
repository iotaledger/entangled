/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__
#define __CONSENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__

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

/**
 *
 *
 *     private final Tangle tangle;
private final boolean testnet;
private final int milestoneStartIndex;

public EntryPointSelectorImpl(Tangle tangle, Milestone milestone, boolean
testnet, int milestoneStartIndex) {
    this.tangle = tangle;
    this.milestone = milestone;

    this.testnet = testnet;
    this.milestoneStartIndex = milestoneStartIndex;
}
 * @param depth
 * @return
 */

typedef struct walker_validator_t {
  tangle_t *tangle;
  milestone_t *milestone;
  ledger_validator_t *lv;
  // private final TransactionValidator transactionValidator;
} walker_validator_t;

extern retcode_t iota_consensus_walker_validator_init(
    walker_validator_t *const wv, tangle_t *const tangle,
    milestone_t *const milestone, ledger_validator_t *const lv);

extern retcode_t iota_consensus_walker_validator_is_valid(
    walker_validator_t *const wv, trit_array_p const tx_hash);

extern retcode_t iota_consensus_walker_validator_below_max_depth(
    walker_validator_t *const wv, trit_array_p const tip, size_t const depth);

extern retcode_t iota_consensus_walker_validator_destroy(
    walker_validator_t *const wv);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__
