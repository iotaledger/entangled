/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_VIEW_MODELS_MILESTONE_H__
#define __CONSENSUS_VIEW_MODELS_MILESTONE_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/model/transaction.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef trit_array_p milestone_view_t;

extern retcode_t iota_consensus_find_closest_next_milestone(
    tangle_t* const tangle, bool const testnet, size_t const milestone_index,
    size_t const milestone_start_index,
    milestone_view_t const closest_milestone_found);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_VIEW_MODELS_MILESTONE_H__
