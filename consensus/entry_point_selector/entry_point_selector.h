/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__
#define __COMMON_CONCENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"

#include "common/storage/connection.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
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

typedef struct entry_point_selector_t {
  tangle_t *tangle;
  milestone_t milestone;
  bool testnet;
  size_t start_index;
} entry_point_selector_t;

extern retcode_t iota_consensus_get_entry_point(
    const entry_point_selector_t *ep_selector, size_t depth, trit_array_p ep);

extern retcode_t iota_consensus_entry_point_selector_init(
    bool testnet, entry_point_selector_t *ep_selector);

extern retcode_t iota_consensus_entry_point_selector_destroy(
    entry_point_selector_t *ep_selector);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__
