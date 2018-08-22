/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_WALKER_WALKER_H__
#define __COMMON_CONCENSUS_WALKER_WALKER_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"

#include "common/storage/connection.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/milestone/milestone.h"
#include "consensus/tangle/tangle.h"
#include "consensus/walker_validator/walker_validator.h"

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

typedef struct walker_t {
  tangle_t *tangle;
  double alpha;
} walker_t;

extern retcode_t iota_consensus_walker_init(const tangle_t *tangle,
                                            double alpha, walker_t *walker);
extern retcode_t iota_consensus_walker_destroy(walker_t *walker);

extern retcode_t iota_consensus_walker_walk(const walker_t *walker,
                                            const trit_array_p ep,
                                            cw_map_t ratings,
                                            const walker_validator_t *wv,
                                            trit_array_p tip);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_WALKER_WALKER_H__
