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
    const tangle_t *tangle, const milestone_t *milestone,
    const ledger_validator_t *lv, walker_validator_t *wv);

extern retcode_t iota_consensus_walker_validator_destroy(
    walker_validator_t *wv);

extern retcode_t iota_consensus_walker_validator_is_valid(
    const walker_validator_t *wv, trit_array_p tx_hash);

extern retcode_t iota_consensus_walker_validator_below_max_depth(
    const walker_validator_t *wv, trit_array_p tip, size_t depth);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_WALKER_VALIDATOR_WALKER_VALIDATOR_H__
