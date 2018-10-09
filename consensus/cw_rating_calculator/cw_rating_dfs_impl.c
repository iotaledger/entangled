/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "common/storage/pack.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "utils/containers/bitset.h"
#include "utils/logger_helper.h"

static retcode_t cw_rating_dfs_do_dfs_from_db(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    hash_to_indexed_hash_set_map_t *tx_to_approvers, size_t *subtangle_size);

static retcode_t cw_rating_dfs_do_dfs_light(
    hash_to_indexed_hash_set_map_t tx_to_approvers, flex_trit_t *ep,
    bitset_t *visited_bitset, size_t *subtangle_size);

void init_cw_calculator_dfs(cw_rating_calculator_base_t *calculator) {
  calculator->vtable = cw_topological_vtable;
}

retcode_t cw_rating_calculate_dfs(const cw_rating_calculator_t *const cw_calc,
                                  trit_array_p entry_point,
                                  cw_calc_result *out) {
  retcode_t res;

  out->tx_to_approvers = NULL;
  out->cw_ratings = NULL;
  hash_to_indexed_hash_set_entry_t *curr_hash_to_approvers_entry = NULL;
  hash_to_indexed_hash_set_entry_t *tmp_hash_to_approvers_entry = NULL;
  size_t sub_tangle_size;
  size_t max_subtangle_size;
  size_t bitset_size;

  if (!entry_point) {
    return RC_OK;
  }

  res = cw_rating_dfs_do_dfs_from_db(
      cw_calc, entry_point, &out->tx_to_approvers, &max_subtangle_size);

  if (res != RC_OK) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID,
              "Failed in DFS from DB, error code is: %" PRIu64 "\n", res);
    return RC_CONSENSUS_CW_FAILED_IN_DFS_FROM_DB;
  }

  // Insert first "ratings" entry
  if ((res = hash_int_map_add(&out->cw_ratings, entry_point->trits,
                              max_subtangle_size))) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID,
              "Failed adding entrypoint into map\n");
    return res;
  }

  if (max_subtangle_size <= 1) {
    return RC_OK;
  }

  bitset_size = bistset_required_size(max_subtangle_size);
  uint64_t visited_raw_bits[bitset_size];
  bitset_t visited_txs_bitset = {.raw_bits = visited_raw_bits,
                                 .bitset_integer_index = 0,
                                 .bitset_relative_index = 0,
                                 .size = bitset_size};

  HASH_ITER(hh, out->tx_to_approvers, curr_hash_to_approvers_entry,
            tmp_hash_to_approvers_entry) {
    if (curr_hash_to_approvers_entry->idx == 0) {
      continue;
    }

    bitset_reset(&visited_txs_bitset);

    flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];
    memcpy(curr_hash, curr_hash_to_approvers_entry->hash, FLEX_TRIT_SIZE_243);
    res = cw_rating_dfs_do_dfs_light(out->tx_to_approvers, curr_hash,
                                     &visited_txs_bitset, &sub_tangle_size);

    if (res != RC_OK) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in light DFS, error code is: %" PRIu64 "\n", res);
      return RC_CONSENSUS_CW_FAILED_IN_LIGHT_DFS;
    }

    if ((res =
             hash_int_map_add(&out->cw_ratings, curr_hash, sub_tangle_size))) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in light DFS, error code is: %" PRIu64 "\n", res);
      return res;
    }
  }

  return RC_OK;
}

static retcode_t cw_rating_dfs_do_dfs_from_db(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    hash_to_indexed_hash_set_map_t *tx_to_approvers, size_t *subtangle_size) {
  hash_to_indexed_hash_set_entry_t *curr_tx = NULL;
  size_t curr_approver_index;
  retcode_t res = RC_OK;
  iota_stor_pack_t pack;
  *subtangle_size = 0;

  if ((res = hash_pack_init(&pack, 10)) != RC_OK) {
    return res;
  }

  hash_queue_t queue = NULL;
  if ((res = hash_queue_push(&queue, entry_point->trits))) {
    return res;
  }

  flex_trit_t *curr_tx_hash = NULL;

  while (!hash_queue_empty(queue)) {
    curr_tx_hash = hash_queue_peek(queue);
    pack.num_loaded = 0;
    pack.insufficient_capacity = false;
    res = iota_tangle_transaction_load_hashes_of_approvers(cw_calc->tangle,
                                                           curr_tx_hash, &pack);

    if (res != RC_OK) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in loading approvers, error code is: %" PRIu64 "\n",
                res);
      return res;
    }

    if (!hash_to_indexed_hash_set_map_contains(tx_to_approvers, curr_tx_hash)) {
      hash_to_indexed_hash_set_map_add_new_set(tx_to_approvers, curr_tx_hash,
                                               &curr_tx, (*subtangle_size)++);

      while (pack.num_loaded > 0) {
        curr_approver_index = --pack.num_loaded;
        // Add each found approver to the currently traversed tx

        if (!hash_to_indexed_hash_set_map_contains(
                tx_to_approvers,
                ((trit_array_p)pack.models[curr_approver_index])->trits)) {
          hash_queue_push(
              &queue, ((trit_array_p)pack.models[curr_approver_index])->trits);
        }

        if ((res = hash_set_add(
                 &curr_tx->approvers,
                 ((trit_array_p)pack.models[pack.num_loaded])->trits))) {
          return res;
        }
      }
    }
    hash_queue_pop(&queue);
  }

  hash_pack_free(&pack);
  hash_queue_free(&queue);

  return res;
}

static retcode_t cw_rating_dfs_do_dfs_light(
    hash_to_indexed_hash_set_map_t tx_to_approvers, flex_trit_t *ep,
    bitset_t *visited_bitset, size_t *subtangle_size) {
  *subtangle_size = 0;
  flex_trit_t *curr_hash = NULL;
  hash_to_indexed_hash_set_entry_t *curr_tx_entry = NULL;
  hash_set_entry_t *curr_direct_approver = NULL;
  hash_set_entry_t *tmp_direct_approver = NULL;
  retcode_t ret;

  hash_queue_t queue = NULL;
  if ((ret = hash_queue_push(&queue, ep))) {
    return ret;
  }

  while (!hash_queue_empty(queue)) {
    curr_hash = hash_queue_peek(queue);

    HASH_FIND(hh, tx_to_approvers, curr_hash, FLEX_TRIT_SIZE_243,
              curr_tx_entry);

    hash_queue_pop(&queue);

    if (!curr_tx_entry) {
      continue;
    }

    if (bitset_is_set(visited_bitset, curr_tx_entry->idx)) {
      continue;
    }
    ++(*subtangle_size);

    bitset_set_true(visited_bitset, curr_tx_entry->idx);

    HASH_ITER(hh, curr_tx_entry->approvers, curr_direct_approver,
              tmp_direct_approver) {
      if (ret = hash_queue_push(&queue, curr_direct_approver->hash)) {
        return ret;
      }
    }
  }

  hash_queue_free(&queue);
  return RC_OK;
}
