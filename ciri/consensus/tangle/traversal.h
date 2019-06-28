/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_TANGLE_TRAVERSALS_H__
#define __UTILS_TANGLE_TRAVERSALS_H__

#include "ciri/consensus/tangle/tangle.h"
#include "common/model/transaction.h"
#include "utils/containers/hash/hash243_set.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A function to execute when a new transaction is visited
 *
 * @param tangle The tangle on which we traverse
 * @param hash The hash of the currently visited transaction
 * @param pack The pack might contain the transaction with the previous `hash`
 * param
 * @param data Additional data
 * @param should_branch Should branch from current transaction
 * @param data should_stop Should stop the traversal
 *
 * @return error value.
 */
typedef retcode_t (*tangle_traversal_functor)(flex_trit_t *const hash, iota_stor_pack_t *pack, void *data,
                                              bool *should_branch, bool *should_stop);

/**
 * Traverse the Tangle (DFS) from an entry point and to its past
 *
 * @param tangle The tangle on which we traverse
 * @param func The operation to do when new transaction is visited
 * @param entry_point Where should the traversal begin from
 * @param data Additional data, (might want to pass to func)
 *
 * @return error value.
 */
retcode_t tangle_traversal_dfs_to_past(tangle_t const *const tangle, tangle_traversal_functor func,
                                       flex_trit_t const *const entry_point, flex_trit_t const *const genesis_hash,
                                       hash243_set_t *analyzed_hashes_param, void *data);

/**
 * Traverse the Tangle (DFS) from an entry point and to its future
 *
 * @param tangle The tangle on which we traverse
 * @param func The operation to do when new transaction is visited
 * @param entry_point Where should the traversal begin from
 * @param data Additional data, (might want to pass to func)
 *
 * @return error value.
 */
retcode_t tangle_traversal_dfs_to_future(tangle_t const *const tangle, tangle_traversal_functor func,
                                         flex_trit_t const *const entry_point, hash243_set_t *analyzed_hashes_param,
                                         void *data);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_TANGLE_TRAVERSALS_H__
