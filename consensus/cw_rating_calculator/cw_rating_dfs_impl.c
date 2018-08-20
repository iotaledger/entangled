/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "common/storage/packs.h"

/*
 * utarray definitions
 */

void uthash_trit_hash_array_copy(void *_dst, const void *_src) {
  flex_trit_t *dst = (flex_trit_t *)_dst;
  flex_trit_t *src = (flex_trit_t *)_src;
  memcpy(dst, src, FLEX_TRIT_SIZE_243);
}

UT_icd cw_stack_trit_array_hash_icd = {FLEX_TRIT_SIZE_243, 0,
                                       uthash_trit_hash_array_copy, 0};

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
  hash_to_direct_approvers_entry_t *currHashToApproversEntry = NULL;
  hash_to_direct_approvers_entry_t *tmpHashToApproversEntry = NULL;
  hash_to_direct_approvers_entry_t *firstHashToApproversEntry = NULL;
  hash_entry_t *currDirectApprover = NULL;
  hash_entry_t *tmpDirectApprover = NULL;
  size_t subTangleSize;
  size_t maxSubTangleSize;

  if (!entry_point) {
    return RC_OK;
  }

  res = cw_rating_dfs_do_dfs_from_db(cw_calc, entry_point,
                                     &out->tx_to_approvers, &maxSubTangleSize);

  if (res != RC_OK) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID,
              "Failed in DFS from DB, error code is: %\" PRIu64 \"", res);
    return RC_CONSENSUS_CW_FAILED_IN_DFS_FROM_DB;
  }

  // Insert first "ratings" entry
  cw_entry = (cw_entry_t *)malloc(sizeof(cw_entry_t));
  memcpy(cw_entry->hash, entry_point->trits, FLEX_TRIT_SIZE_243);
  cw_entry->cw = maxSubTangleSize;
  HASH_ADD(hh, out->cw_ratings, hash, FLEX_TRIT_SIZE_243, cw_entry);

  if (maxSubTangleSize <= 1) {
    return RC_OK;
  }

  int64_t visitedTxsBitset[subTangleSize / (sizeof(int64_t) * 8) + 1];

  HASH_ITER(hh, out->tx_to_approvers, currHashToApproversEntry,
            tmpHashToApproversEntry) {
    if (currHashToApproversEntry->idx == 0) {
      continue;
    }
    // Each iteration is a DFS, so we must clean "visitedTxsBitset" because no
    // tx has been visited yet
    memset(visitedTxsBitset, maxSubTangleSize / (sizeof(int64_t) * 8), 0);
    res = cw_rating_dfs_do_dfs_light(out->tx_to_approvers,
                                     currHashToApproversEntry->hash,
                                     visitedTxsBitset, &subTangleSize);

    if (res != RC_OK) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in light DFS, error code is: %\" PRIu64 \"", res);
      return RC_CONSENSUS_CW_FAILED_IN_LIGHT_DFS;
    }

    cw_entry = (cw_entry_t *)malloc(sizeof(cw_entry_t));
    memcpy(cw_entry->hash, entry_point->trits, FLEX_TRIT_SIZE_243);
    cw_entry->cw = subTangleSize;
    HASH_ADD(hh, out->cw_ratings, hash, FLEX_TRIT_SIZE_243,
             currHashToApproversEntry);
  }

  return RC_OK;
}

retcode_t cw_rating_dfs_do_dfs_from_db(
    const cw_rating_calculator_t *const cw_calc, trit_array_p entry_point,
    hash_to_direct_approvers_map_t *txToApprovers, size_t *subTangleSize) {
  hash_to_direct_approvers_entry_t *currTx = NULL;
  hash_to_direct_approvers_entry_t *currApproverTx = NULL;
  hash_entry_t *currDirectApprover = NULL;
  trit_array_p currTxTritArray = NULL;
  size_t currApproverIndex;

  retcode_t res = RC_OK;
  iota_hashes_pack pack;
  *subTangleSize = 0;

  if (res = hash_pack_init(&pack, 10) != RC_OK) {
    return res;
  }

  UT_array *stack = NULL;
  utarray_new(stack, &cw_stack_trit_array_hash_icd);
  utarray_push_back(stack, entry_point->trits);

  currTxTritArray = trit_array_new(FLEX_TRIT_SIZE_243);
  flex_trit_t *currTxHash = NULL;

  while (utarray_len(stack)) {
    currTxHash = utarray_back(stack);
    utarray_pop_back(stack);
    pack.num_loaded = 0;
    trit_array_set_trits(currTxTritArray, currTxHash, NUM_TRITS_HASH);
    res = iota_tangle_load_approvers_hashes(cw_calc->tangle, currTxTritArray,
                                            &pack);

    if (res != RC_OK) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in loading approvers, error code is: %\" PRIu64 \"",
                res);
      return res;
    } else if (pack.insufficient_capacity) {
      res = hash_pack_resize(&pack, 2);
      if (res != RC_OK) {
        return res;
      }
      utarray_push_back(stack, currTxHash);
      continue;
    }

    if (*txToApprovers) {
      HASH_FIND(hh, *txToApprovers, currTxHash, FLEX_TRIT_SIZE_243, currTx);
    }

    if (!currTx) {
      currTx = (hash_to_direct_approvers_entry_t *)malloc(
          sizeof(hash_to_direct_approvers_entry_t));

      currTx->approvers = NULL;
      currTx->idx = (*subTangleSize)++;
      memcpy(currTx->hash, currTxHash, FLEX_TRIT_SIZE_243);

      HASH_ADD(hh, *txToApprovers, hash, FLEX_TRIT_SIZE_243, currTx);

      while (pack.num_loaded > 0) {
        currApproverIndex = --pack.num_loaded;
        // Add each found approver to the currently traversed tx
        currDirectApprover = (hash_entry_t *)malloc(sizeof(hash_entry_t));
        memcpy(currDirectApprover->hash, pack.hashes[pack.num_loaded]->trits,
               FLEX_TRIT_SIZE_243);

        HASH_ADD(hh, currTx->approvers, hash, FLEX_TRIT_SIZE_243,
                 currDirectApprover);

        // push approver to stack only if it hasn't been found yet
        HASH_FIND(hh, *txToApprovers, pack.hashes[currApproverIndex]->trits,
                  FLEX_TRIT_SIZE_243, currApproverTx);
        if (!currApproverTx) {
          utarray_push_back(stack, pack.hashes[currApproverIndex]->trits);
        }
      }
    }
  }

  hash_pack_free(&pack);
  utarray_free(stack);

  return res;
}

retcode_t cw_rating_dfs_do_dfs_light(
    hash_to_direct_approvers_map_t txToApprovers, flex_trit_t *ep,
    int64_t *visitedBitSet, size_t *subTangleSize) {
  *subTangleSize = 1;
  flex_trit_t *curr_hash_prefix = NULL;
  hash_to_direct_approvers_entry_t *visitedApproversEntry = NULL;
  size_t numApprover;

  int txBitSetIntegerIndex;
  int txBitSetWithinIntegerRelativeIndex;

  UT_array *stack = NULL;
  utarray_new(stack, &cw_stack_trit_array_hash_icd);
  utarray_push_back(stack, ep);

  while (utarray_len(stack)) {
    curr_hash_prefix = utarray_back(stack);
    utarray_pop_back(stack);

    HASH_FIND(hh, txToApprovers, curr_hash_prefix, FLEX_TRIT_SIZE_243,
              visitedApproversEntry);

    if (!visitedApproversEntry || visitedApproversEntry->idx == 0) {
      continue;
    }

    txBitSetIntegerIndex =
        visitedApproversEntry->idx / (sizeof(*visitedBitSet) * 8);
    txBitSetWithinIntegerRelativeIndex =
        visitedApproversEntry->idx % (sizeof(*visitedBitSet) * 8);

    if (visitedBitSet[txBitSetIntegerIndex] &
        (1 << txBitSetWithinIntegerRelativeIndex)) {
      continue;
    }
    ++(subTangleSize);

    visitedBitSet[txBitSetIntegerIndex] &= (1 << visitedApproversEntry->idx);

    size_t numApprovers = HASH_COUNT(visitedApproversEntry->approvers);
    for (numApprover = 0; numApprover < numApprovers; ++numApprover) {
      HASH_FIND(hh, txToApprovers, visitedApproversEntry->hash,
                FLEX_TRIT_SIZE_243, visitedApproversEntry);

      if (visitedBitSet[txBitSetIntegerIndex] &
          (1 << visitedApproversEntry->idx) == 0) {
        utarray_push_back(stack, visitedApproversEntry->hash);
      }
    }
  }

  utarray_free(stack);
  return RC_OK;
}