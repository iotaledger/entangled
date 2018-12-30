/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/utils/tangle_simulator.h"

retcode_t tangle_simulator_add_transaction_recalc_ratings(
    size_t max_subtangle_size, flex_trit_t const *const hash,
    flex_trit_t const *const branch, flex_trit_t const *const trunk,
    hash_to_indexed_hash_set_map_t *const tx_to_approvers,
    hash_to_int64_t_map_t *const cw_ratings) {
  retcode_t res;

  hash_to_indexed_hash_set_entry_t *indexed_branch_entry = NULL;
  hash_to_indexed_hash_set_entry_t *indexed_trunk_entry = NULL;

  if ((res = hash_to_indexed_hash_set_map_find(
           tx_to_approvers, branch, &indexed_branch_entry)) != RC_OK) {
    return res;
  }

  if ((res = hash_to_indexed_hash_set_map_find(
           tx_to_approvers, trunk, &indexed_trunk_entry)) != RC_OK) {
    return res;
  }

  // tx_to_approvers should contain only solid transactions
  if (indexed_branch_entry && indexed_trunk_entry) {
    if ((res = hash243_set_add(&indexed_branch_entry->approvers, hash))) {
      return res;
    }
    if ((res = hash243_set_add(&indexed_trunk_entry->approvers, hash))) {
      return res;
    }
  }

  return cw_rating_calculate_dfs_ratings_from_approvers_map(
      max_subtangle_size + 2, *tx_to_approvers, cw_ratings, false);
}