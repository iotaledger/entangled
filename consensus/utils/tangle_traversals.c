/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "tangle_traversals.h"
#include "common/storage/defs.h"
#include "common/storage/pack.h"

retcode_t tangle_traversal_dfs_to_genesis(tangle_t *const tangle,
                                          tangle_traversal_functor func,
                                          flex_trit_t *const entry_point,
                                          void *data) {
  retcode_t ret = RC_OK;
  hash243_stack_t non_analyzed_hashes = NULL;
  hash243_set_t analyzed_hashes = NULL;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  struct _trit_array tx_hash = {NULL, NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  if ((ret = hash243_stack_push(&non_analyzed_hashes, entry_point)) != RC_OK) {
  }
  bool should_branch;
  bool should_stop;

  while (non_analyzed_hashes != NULL) {
    tx_hash.trits = hash243_stack_peek(non_analyzed_hashes);
    if (!hash243_set_contains(&analyzed_hashes, tx_hash.trits)) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH,
                                              &tx_hash, &pack)) != RC_OK) {
        break;
      }

      if ((ret = func(tangle, tx_hash.trits, &pack, data, &should_branch,
                      &should_stop)) != RC_OK) {
        break;
      }

      if (should_stop) {
        break;
      }

      if (should_branch) {
        // TODO messageQ publish
        if ((ret = hash243_stack_push(&non_analyzed_hashes, tx.trunk)) !=
            RC_OK) {
          break;
        }
        if ((ret = hash243_stack_push(&non_analyzed_hashes, tx.branch)) !=
            RC_OK) {
          break;
        }
      }
      if ((ret = hash243_set_add(&analyzed_hashes, tx_hash.trits)) != RC_OK) {
        break;
      }
    }
    hash243_stack_pop(&non_analyzed_hashes);
  }

  hash243_stack_free(&non_analyzed_hashes);
  hash243_set_free(&analyzed_hashes);

  return ret;
}
