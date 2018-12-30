/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_TANGLE_SIMULATOR_H__
#define __UTILS_TANGLE_SIMULATOR_H__

#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Add transaction to tx_to_approvers and recalc ratings
 * which is considered for the tipselection, keep in mind, whenever this
 * function is called, this param should be incremented
 *
 * @param max_subtangle_size The tangle size from the entry point [in]
 * @param hash The new transaction's hash
 * @param hash The new transaction's branch
 * @param hash The new transaction's trunk
 * @param tx_to_approvers The mapping between transactions to their direct
 * approvers [in,out]
 * @param cw_ratings The tangle ratings [out]
 *
 * @return error value.
 */
retcode_t tangle_simulator_add_transaction_recalc_ratings(
    size_t max_subtangle_size, flex_trit_t const *const hash,
    flex_trit_t const *const branch, flex_trit_t const *const trunk,
    hash_to_indexed_hash_set_map_t *const tx_to_approvers,
    hash_to_int64_t_map_t *const cw_ratings);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_TANGLE_SIMULATOR_H__
