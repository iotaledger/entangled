/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_DFS_IMPL_H__
#define __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_DFS_IMPL_H__

#include "utarray.h"
#include "uthash.h"

#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * (E ~ 2*V - because each transaction has two outcoming edges)
 */

extern retcode_t cw_rating_calculate_dfs(cw_rating_calculator_t const *const cw_calc, tangle_t *const tangle,
                                         flex_trit_t const *const entry_point, cw_calc_result *const out);

static cw_calculator_vtable cw_topological_vtable = {
    .cw_rating_calculate = cw_rating_calculate_dfs,
};

#ifdef __cplusplus
}
#endif

#endif  //__CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_DFS_IMPL_H__
