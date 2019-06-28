/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "common/storage/pack.h"
#include "utils/containers/bitset.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define CW_RATING_CALCULATOR_LOGGER_ID "cw_rating_calculator"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t cw_rating_dfs_do_dfs_from_db(tangle_t *const tangle, flex_trit_t const *const entry_point,
                                              hash_to_indexed_hash_set_map_t *tx_to_approvers, uint64_t *subtangle_size,
                                              int64_t subtangle_before_timestamp) {
  hash_to_indexed_hash_set_entry_t *curr_tx = NULL;
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;
  hash243_stack_t stack = NULL;
  flex_trit_t *curr_tx_hash = NULL;

  *subtangle_size = 0;

  if ((ret = hash_pack_init(&pack, 10)) != RC_OK) {
    goto done;
  }

  if ((ret = hash243_stack_push(&stack, entry_point)) != RC_OK) {
    goto done;
  }

  while (!hash243_stack_empty(stack)) {
    curr_tx_hash = hash243_stack_peek(stack);

    if (!hash_to_indexed_hash_set_map_contains(tx_to_approvers, curr_tx_hash)) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load_hashes_of_approvers(tangle, curr_tx_hash, &pack,
                                                                  subtangle_before_timestamp)) != RC_OK) {
        log_error(logger_id, "Failed in loading approvers, error code is: %" PRIu64 "\n", ret);
        goto done;
      }
      if ((ret = hash_to_indexed_hash_set_map_add_new_set(tx_to_approvers, curr_tx_hash, &curr_tx,
                                                          (*subtangle_size)++)) != RC_OK) {
        goto done;
      }
      hash243_stack_pop(&stack);
      while (pack.num_loaded > 0) {
        curr_tx_hash = ((flex_trit_t *)pack.models[--pack.num_loaded]);
        // Add each found approver to the currently traversed tx
        if ((ret = hash243_stack_push(&stack, curr_tx_hash)) != RC_OK) {
          goto done;
        }
        if ((ret = hash243_set_add(&curr_tx->approvers, curr_tx_hash)) != RC_OK) {
          goto done;
        }
      }
      continue;
    }
    hash243_stack_pop(&stack);
  }

done:
  hash_pack_free(&pack);
  hash243_stack_free(&stack);

  return ret;
}

static retcode_t cw_rating_dfs_do_dfs_light(hash_to_indexed_hash_set_map_t tx_to_approvers, flex_trit_t *ep,
                                            bitset_t *visited_bitset, uint64_t *subtangle_size) {
  *subtangle_size = 0;
  flex_trit_t *curr_hash = NULL;
  hash_to_indexed_hash_set_entry_t *curr_tx_entry = NULL;
  retcode_t ret = RC_OK;
  hash243_stack_t stack = NULL;

  if ((ret = hash243_stack_push(&stack, ep)) != RC_OK) {
    goto done;
  }

  while (!hash243_stack_empty(stack)) {
    curr_hash = hash243_stack_peek(stack);

    HASH_FIND(hh, tx_to_approvers, curr_hash, FLEX_TRIT_SIZE_243, curr_tx_entry);

    hash243_stack_pop(&stack);

    if (!curr_tx_entry) {
      continue;
    }

    if (bitset_is_set(visited_bitset, curr_tx_entry->idx)) {
      continue;
    }
    ++(*subtangle_size);

    bitset_set_true(visited_bitset, curr_tx_entry->idx);

    if ((ret = hash243_set_for_each(&curr_tx_entry->approvers, (hash243_on_container_func)hash243_stack_push,
                                    &stack)) != RC_OK) {
      goto done;
    }
  }

done:
  hash243_stack_free(&stack);

  return ret;
}

/*
 * Public functions
 */

void init_cw_calculator_dfs(cw_rating_calculator_base_t *calculator) {
  logger_id = logger_helper_enable(CW_RATING_CALCULATOR_LOGGER_ID, LOGGER_DEBUG, true);
  calculator->vtable = cw_topological_vtable;
}

retcode_t cw_rating_calculate_dfs(cw_rating_calculator_t const *const cw_calc, tangle_t *const tangle,
                                  flex_trit_t const *const entry_point, cw_calc_result *const out) {
  retcode_t ret = RC_OK;
  hash_to_indexed_hash_set_entry_t *curr_hash_to_approvers_entry = NULL;
  hash_to_indexed_hash_set_entry_t *tmp_hash_to_approvers_entry = NULL;
  uint64_t sub_tangle_size = 0;
  uint64_t max_subtangle_size = 0;
  uint64_t bitset_size = 0;
  UNUSED(cw_calc);

  out->cw_ratings = NULL;
  out->tx_to_approvers = NULL;

  if (!entry_point) {
    return RC_NULL_PARAM;
  }

  if ((ret = cw_rating_dfs_do_dfs_from_db(tangle, entry_point, &out->tx_to_approvers, &max_subtangle_size, 0)) !=
      RC_OK) {
    log_error(logger_id, "Failed in DFS from DB, error code is: %" PRIu64 "\n", ret);
    return RC_CW_FAILED_IN_DFS_FROM_DB;
  }

  // Insert first "ratings" entry
  if ((ret = hash_to_int64_t_map_add(&out->cw_ratings, entry_point, max_subtangle_size)) != RC_OK) {
    log_error(logger_id, "Failed adding entrypoint into map\n");
    return ret;
  }

  if (max_subtangle_size <= 1) {
    return RC_OK;
  }

  bitset_size = bistset_required_size(max_subtangle_size);

  {
    uint64_t visited_raw_bits[bitset_size];
    bitset_t visited_txs_bitset = {
        .raw_bits = visited_raw_bits, .bitset_integer_index = 0, .bitset_relative_index = 0, .size = bitset_size};
    flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];

    HASH_ITER(hh, out->tx_to_approvers, curr_hash_to_approvers_entry, tmp_hash_to_approvers_entry) {
      if (curr_hash_to_approvers_entry->idx == 0) {
        continue;
      }

      bitset_reset(&visited_txs_bitset);
      memcpy(curr_hash, curr_hash_to_approvers_entry->hash, FLEX_TRIT_SIZE_243);
      if ((ret = cw_rating_dfs_do_dfs_light(out->tx_to_approvers, curr_hash, &visited_txs_bitset, &sub_tangle_size)) !=
          RC_OK) {
        log_error(logger_id, "Failed in light DFS, error code is: %" PRIu64 "\n", ret);
        return RC_CW_FAILED_IN_LIGHT_DFS;
      }

      if ((ret = hash_to_int64_t_map_add(&out->cw_ratings, curr_hash, sub_tangle_size))) {
        log_error(logger_id, "Failed in light DFS, error code is: %" PRIu64 "\n", ret);
        return ret;
      }
    }
  }

  return ret;
}
