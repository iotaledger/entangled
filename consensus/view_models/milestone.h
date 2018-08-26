/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_VIEW_MODELS_MILESTONE_H__
#define __COMMON_CONCENSUS_VIEW_MODELS_MILESTONE_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"
#include "common/model/transaction.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef trit_array_p milestone_view_t;

retcode_t iota_consensus_find_closest_next_milestone(
    const tangle_t* tangle, size_t index, bool testnet,
    size_t milestone_start_index, milestone_view_t closest_milestone_found);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_VIEW_MODELS_MILESTONE_H__
