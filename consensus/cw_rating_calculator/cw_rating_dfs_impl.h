/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_CW_RATING_CALCULATOR_CW_RATING_TOPOLOGICAL_SORT_IMPL_H__
#define __COMMON_CONCENSUS_CW_RATING_CALCULATOR_CW_RATING_TOPOLOGICAL_SORT_IMPL_H__

#include "consensus/cw_rating_calculator/cw_rating_calculator.h"

#include "utarray.h"
#include "uthash.h"

#define CW_RATING_CALCULATOR_LOGGER_ID "consensus_cw_rating_calculator"

void init_cw_calculator_dfs(cw_rating_calculator_base_t *calculator);
/**
 *
 * @param cw_calc - the calculator
 * @param entry_point  - where should the rating calculation start from
 * @param out - a struct containing the ratings and mapping between txs and
 *              their approvers - both should be freed!!!
 * @return retcode_t
 *
 * This implementation does a DFS from entry point to discover all transactions
 * this first DFS is done using storage to load each transaction approvers, and
 * we use this DFS to store transactions loaded from storage in a map (in
 * memory) then, for each transaction, another DFS is performed to discover it's
 * approvers using the map we got from initial DFS Complexity: DFS + (num
 * vertices)*DFS = (E+V) + V*(E+V) ~ V*(E+V)~O(V^2)
 *
 * (E ~ 2*V - because each transaction has two edges)
 */

extern retcode_t cw_rating_calculate_dfs(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    cw_calc_result *out);

static cw_calculator_vtable cw_topological_vtable = {
    .cw_rating_calculate = cw_rating_calculate_dfs,
};

extern retcode_t cw_rating_dfs_do_dfs_from_db(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    hash_to_direct_approvers_map_t *txToApprovers, size_t *subTangleSize);

extern retcode_t cw_rating_dfs_do_dfs_light(
    hash_to_direct_approvers_map_t txToApprovers, flex_trit_t *ep,
    int64_t *visitedBitSet, size_t *subTangleSize);

#endif  //__COMMON_CONCENSUS_CW_RATING_CALCULATOR_CW_RATING_TOPOLOGICAL_SORT_IMPL_H__
