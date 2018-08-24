/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
#define __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/storage/connection.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/milestone/milestone.h"
#include "consensus/model.h"
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

typedef struct ledger_validator_t {
  tangle_t *tangle;
  milestone_t *milestone;
  // private final TransactionRequester transactionRequester;
  // private final MessageQ messageQ;
  // private volatile int numberOfConfirmedTransactions;
} ledger_validator_t;

extern retcode_t iota_consensus_ledger_validator_init(
    tangle_t *const tangle, milestone_t *const milestone,
    ledger_validator_t *const lv);

extern retcode_t iota_consensus_ledger_validator_destroy(
    ledger_validator_t *const lv);

extern retcode_t iota_consensus_ledeger_validator_validate(
    const ledger_validator_t *lv, const tips_pair *pair);

// TODO - complete

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
