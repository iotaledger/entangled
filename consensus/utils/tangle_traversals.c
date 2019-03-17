/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/utils/tangle_traversals.h"
#include "common/storage/defs.h"
#include "common/storage/pack.h"
#include "utils/containers/hash/hash243_stack.h"

retcode_t tangle_traversal_dfs_to_genesis(tangle_t const *const tangle, tangle_traversal_functor func,
                                          flex_trit_t const *const entry_point, flex_trit_t const *const genesis_hash,
                                          hash243_set_t *const analyzed_hashes_param, void *data) {
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

  bool should_branch;
  bool should_stop;

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
