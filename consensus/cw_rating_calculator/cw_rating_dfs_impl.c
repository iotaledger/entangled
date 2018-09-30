/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "common/storage/pack.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "utils/logger_helper.h"

static retcode_t cw_rating_dfs_do_dfs_from_db(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    hash_to_direct_approvers_map_t *tx_to_approvers, size_t *subtangle_size);

static retcode_t cw_rating_dfs_do_dfs_light(
    hash_to_direct_approvers_map_t tx_to_approvers, flex_trit_t *ep,
    int64_t *visited_bitset, size_t *subtangle_size);

/*
 * utarray definitions
 */

UT_icd cw_stack_trit_array_hash_icd = {FLEX_TRIT_SIZE_243, 0, 0, 0};

void init_cw_calculator_dfs(cw_rating_calculator_base_t *calculator) {
  calculator->vtable = cw_topological_vtable;
}

retcode_t cw_rating_calculate_dfs(const cw_rating_calculator_t *const cw_calc,
                                  trit_array_p entry_point,
                                  cw_calc_result *out) {
  retcode_t res;

  out->tx_to_approvers = NULL;
  out->cw_ratings = NULL;
  cw_entry_t *cw_entry = NULL;
  hash_to_direct_approvers_entry_t *curr_hash_to_approvers_entry = NULL;
  hash_to_direct_approvers_entry_t *tmp_hash_to_approvers_entry = NULL;
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
  cw_entry = (cw_entry_t *)malloc(sizeof(cw_entry_t));
  if (cw_entry == NULL) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID, "Failed in memory allocation\n");
    return RC_CONSENSUS_OOM;
  }
  memcpy(cw_entry->hash, entry_point->trits, FLEX_TRIT_SIZE_243);
  cw_entry->cw = max_subtangle_size;
  HASH_ADD(hh, out->cw_ratings, hash, FLEX_TRIT_SIZE_243, cw_entry);

  if (max_subtangle_size <= 1) {
    return RC_OK;
  }

  bitset_size = max_subtangle_size / (sizeof(int64_t) * 8) + 1;
  int64_t visitedTxsBitset[bitset_size];

  HASH_ITER(hh, out->tx_to_approvers, curr_hash_to_approvers_entry,
            tmp_hash_to_approvers_entry) {
    if (curr_hash_to_approvers_entry->idx == 0) {
      continue;
    }

    memset(visitedTxsBitset, 0, bitset_size * sizeof(int64_t));
    flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];
    memcpy(curr_hash, curr_hash_to_approvers_entry->hash, FLEX_TRIT_SIZE_243);
    res = cw_rating_dfs_do_dfs_light(out->tx_to_approvers, curr_hash,
                                     visitedTxsBitset, &sub_tangle_size);

    if (res != RC_OK) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in light DFS, error code is: %" PRIu64 "\n", res);
      return RC_CONSENSUS_CW_FAILED_IN_LIGHT_DFS;
    }

    cw_entry = (cw_entry_t *)malloc(sizeof(cw_entry_t));
    if (cw_entry == NULL) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in memory allocation\n");
      return RC_CONSENSUS_OOM;
    }
    memcpy(cw_entry->hash, curr_hash, FLEX_TRIT_SIZE_243);
    cw_entry->cw = sub_tangle_size;
    HASH_ADD(hh, out->cw_ratings, hash, FLEX_TRIT_SIZE_243, cw_entry);
  }

  return RC_OK;
}

static retcode_t cw_rating_dfs_do_dfs_from_db(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    hash_to_direct_approvers_map_t *tx_to_approvers, size_t *subtangle_size) {
  hash_to_direct_approvers_entry_t *curr_tx = NULL;
  hash_to_direct_approvers_entry_t *curr_approver_tx = NULL;
  hash_entry_t *curr_direct_approver = NULL;
  size_t curr_approver_index;

  retcode_t res = RC_OK;
  iota_stor_pack_t pack;
  *subtangle_size = 0;

  if ((res = hash_pack_init(&pack, 10)) != RC_OK) {
    return res;
  }

  UT_array *stack = NULL;
  utarray_new(stack, &cw_stack_trit_array_hash_icd);
  if (stack == NULL) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID, "Failed in memory allocation\n");
    return RC_CONSENSUS_OOM;
  }
  utarray_push_back(stack, entry_point->trits);

  flex_trit_t *curr_tx_hash = NULL;

  while (utarray_len(stack)) {
    curr_tx_hash = (flex_trit_t *)utarray_back(stack);
    utarray_pop_back(stack);
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

    if (*tx_to_approvers) {
      HASH_FIND(hh, *tx_to_approvers, curr_tx_hash, FLEX_TRIT_SIZE_243,
                curr_tx);
    }

    if (!curr_tx) {
      curr_tx = (hash_to_direct_approvers_entry_t *)malloc(
          sizeof(hash_to_direct_approvers_entry_t));
      if (curr_tx == NULL) {
        log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                  "Failed in memory allocation\n");
        return RC_CONSENSUS_OOM;
      }

      curr_tx->approvers = NULL;
      curr_tx->idx = (*subtangle_size)++;
      memcpy(curr_tx->hash, curr_tx_hash, FLEX_TRIT_SIZE_243);

      while (pack.num_loaded > 0) {
        curr_approver_index = --pack.num_loaded;
        // Add each found approver to the currently traversed tx
        curr_direct_approver = (hash_entry_t *)malloc(sizeof(hash_entry_t));
        if (curr_direct_approver == NULL) {
          log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                    "Failed in memory allocation\n");
          return RC_CONSENSUS_OOM;
        }
        memcpy(curr_direct_approver->hash,
               ((trit_array_p)pack.models[pack.num_loaded])->trits,
               FLEX_TRIT_SIZE_243);

        HASH_ADD(hh, curr_tx->approvers, hash, FLEX_TRIT_SIZE_243,
                 curr_direct_approver);

        // push approver to stack only if it hasn't been found yet
        HASH_FIND(hh, *tx_to_approvers,
                  ((trit_array_p)pack.models[curr_approver_index])->trits,
                  FLEX_TRIT_SIZE_243, curr_approver_tx);
        if (!curr_approver_tx) {
          utarray_push_back(
              stack, ((trit_array_p)pack.models[curr_approver_index])->trits);
        }
      }

      HASH_ADD(hh, *tx_to_approvers, hash, FLEX_TRIT_SIZE_243, curr_tx);
    }
  }

  hash_pack_free(&pack);
  utarray_free(stack);

  return res;
}

static retcode_t cw_rating_dfs_do_dfs_light(
    hash_to_direct_approvers_map_t tx_to_approvers, flex_trit_t *ep,
    int64_t *visited_bitset, size_t *subtangle_size) {
  *subtangle_size = 0;
  flex_trit_t *curr_hash = NULL;
  hash_to_direct_approvers_entry_t *curr_tx_entry = NULL;
  hash_entry_t *curr_direct_approver = NULL;
  hash_entry_t *tmp_direct_approver = NULL;

  uint64_t tx_bitset_integer_index;
  uint64_t tx_bitset_within_integer_relative_index;

  UT_array *stack = NULL;
  utarray_new(stack, &cw_stack_trit_array_hash_icd);
  if (stack == NULL) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID, "Failed in memory allocation\n");
    return RC_CONSENSUS_OOM;
  }
  utarray_push_back(stack, ep);

  while (utarray_len(stack)) {
    curr_hash = (flex_trit_t *)utarray_back(stack);
    utarray_pop_back(stack);

    HASH_FIND(hh, tx_to_approvers, curr_hash, FLEX_TRIT_SIZE_243,
              curr_tx_entry);

    if (!curr_tx_entry) {
      continue;
    }

    tx_bitset_integer_index =
        curr_tx_entry->idx / (sizeof(*visited_bitset) * 8);
    tx_bitset_within_integer_relative_index =
        curr_tx_entry->idx % (sizeof(*visited_bitset) * 8);

    if (visited_bitset[tx_bitset_integer_index] &
        (1ULL << tx_bitset_within_integer_relative_index)) {
      continue;
    }
    ++(*subtangle_size);

    visited_bitset[tx_bitset_integer_index] |= (1ULL << curr_tx_entry->idx);

    HASH_ITER(hh, curr_tx_entry->approvers, curr_direct_approver,
              tmp_direct_approver) {
      utarray_push_back(stack, curr_direct_approver->hash);
    }
  }

  utarray_free(stack);
  return RC_OK;
}
