/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/tangle/traversal.h"
#include "common/storage/defs.h"
#include "common/storage/pack.h"
#include "utils/containers/hash/hash243_stack.h"

retcode_t tangle_traversal_dfs_to_past(tangle_t const *const tangle, tangle_traversal_functor func,
                                       flex_trit_t const *const entry_point, flex_trit_t const *const genesis_hash,
                                       hash243_set_t *analyzed_hashes_param, void *data) {
  retcode_t ret = RC_OK;
  hash243_stack_t non_analyzed_hashes = NULL;
  hash243_set_t analyzed_hashes_local = NULL;
  hash243_set_t *analyzed_hashes = analyzed_hashes_param ? analyzed_hashes_param : &analyzed_hashes_local;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  if ((ret = hash243_stack_push(&non_analyzed_hashes, entry_point)) != RC_OK) {
    return ret;
  }
  if ((ret = hash243_set_add(analyzed_hashes, genesis_hash)) != RC_OK) {
    return ret;
  }

  bool should_branch = true;
  bool should_stop = false;

  while (non_analyzed_hashes != NULL) {
    memcpy(hash, hash243_stack_peek(non_analyzed_hashes), FLEX_TRIT_SIZE_243);
    hash243_stack_pop(&non_analyzed_hashes);
    if (!hash243_set_contains(analyzed_hashes, hash)) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load_partial(tangle, hash, &pack,
                                                      PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA)) != RC_OK) {
        break;
      }

      if ((ret = func(hash, &pack, data, &should_branch, &should_stop)) != RC_OK) {
        break;
      }

      if (should_stop) {
        break;
      }

      if (should_branch) {
        if ((ret = hash243_stack_push(&non_analyzed_hashes, transaction_trunk(&tx))) != RC_OK) {
          break;
        }
        if ((ret = hash243_stack_push(&non_analyzed_hashes, transaction_branch(&tx))) != RC_OK) {
          break;
        }
      }
      if ((ret = hash243_set_add(analyzed_hashes, hash)) != RC_OK) {
        break;
      }
    }
  }

  hash243_stack_free(&non_analyzed_hashes);
  if (!analyzed_hashes_param) {
    hash243_set_free(analyzed_hashes);
  }
  return ret;
}

retcode_t tangle_traversal_dfs_to_future(tangle_t const *const tangle, tangle_traversal_functor func,
                                         flex_trit_t const *const entry_point, hash243_set_t *analyzed_hashes_param,
                                         void *data) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t hashes_pack;
  hash243_stack_t non_analyzed_hashes = NULL;
  flex_trit_t curr_tx_hash[FLEX_TRIT_SIZE_243];
  hash243_set_t analyzed_hashes_local = NULL;
  hash243_set_t *analyzed_hashes = analyzed_hashes_param ? analyzed_hashes_param : &analyzed_hashes_local;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, transaction_pack);

  if ((ret = hash243_stack_push(&non_analyzed_hashes, entry_point)) != RC_OK) {
    return ret;
  }

  if ((ret = hash_pack_init(&hashes_pack, 8)) != RC_OK) {
    goto done;
  }

  bool should_branch = true;
  bool should_stop = false;

  while (!hash243_stack_empty(non_analyzed_hashes)) {
    memcpy(curr_tx_hash, hash243_stack_peek(non_analyzed_hashes), FLEX_TRIT_SIZE_243);

    if (!hash243_set_contains(analyzed_hashes, curr_tx_hash)) {
      hash_pack_reset(&transaction_pack);
      if ((ret = iota_tangle_transaction_load_partial(tangle, curr_tx_hash, &transaction_pack,
                                                      PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA)) != RC_OK) {
        break;
      }

      if ((ret = func(curr_tx_hash, &transaction_pack, data, &should_branch, &should_stop)) != RC_OK) {
        break;
      }

      if ((ret = hash243_set_add(analyzed_hashes, curr_tx_hash)) != RC_OK) {
        goto done;
      }

      if (should_stop) {
        break;
      }

      if (should_branch) {
        hash243_stack_pop(&non_analyzed_hashes);
        hash_pack_reset(&hashes_pack);
        if ((ret = iota_tangle_transaction_load_hashes_of_approvers(tangle, curr_tx_hash, &hashes_pack, 0)) != RC_OK) {
          goto done;
        }
        while (hashes_pack.num_loaded > 0) {
          // Add each found approver to the currently traversed tx
          if ((ret = hash243_stack_push(&non_analyzed_hashes,
                                        ((flex_trit_t *)hashes_pack.models[--hashes_pack.num_loaded]))) != RC_OK) {
            goto done;
          }
        }
        continue;
      }
    }
    hash243_stack_pop(&non_analyzed_hashes);
  }

done:
  hash_pack_free(&hashes_pack);
  hash243_stack_free(&non_analyzed_hashes);
  if (!analyzed_hashes_param) {
    hash243_set_free(analyzed_hashes);
  }

  return ret;
}
