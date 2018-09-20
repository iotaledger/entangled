/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/exit_probability_randomizer/walker.h"
#include "utils/logger_helper.h"
#include "utlist.h"

#include <math.h>

#define RANDOM_WALKER_ID "consensus_random_walker"

void init_ep_randomizer_walker(ep_randomizer_t *randomizer) {
  randomizer->base.vtable = random_walk_vtable;
}

static retcode_t random_walker_select_approver_tail(
    const ep_randomizer_t *exit_probability_randomizer, flex_trit_t *curr_tail,
    cw_calc_result *cw_result, const exit_prob_transaction_validator_t *epv,
    trit_array_t *approver_tail_hash, bool *found_approver);

static retcode_t select_approver(
    const ep_randomizer_t *exit_probability_randomizer, cw_map_t cw_ratings,
    hash_set_t *approvers, trit_array_t *approver, bool *has_next_approver);

static retcode_t find_tail_if_valid(
    const ep_randomizer_t *exit_probability_randomizer,
    const trit_array_t *tx_hash, trit_array_t *tail_hash, bool *found_tail);

retcode_t iota_consensus_random_walker_randomize(
    const ep_randomizer_t *exit_probability_randomizer,
    const exit_prob_transaction_validator_t *ep_validator,
    cw_calc_result *cw_result, const trit_array_p ep, trit_array_p tip) {
  retcode_t ret = RC_OK;
  bool is_valid = false;
  flex_trit_t curr_tail_hash[FLEX_TRIT_SIZE_243];
  bool has_next_approver_tail = false;
  size_t num_traversed_tails = 0;
  trit_array_t next_approver_tail;
  flex_trit_t approver_tail_trits[FLEX_TRIT_SIZE_243];
  next_approver_tail.trits = approver_tail_trits;
  next_approver_tail.num_bytes = FLEX_TRIT_SIZE_243;

  ret = iota_consensus_exit_prob_transaction_validator_is_valid(ep_validator,
                                                                ep, &is_valid);
  if (ret) {
    log_error(RANDOM_WALKER_ID,
              "Failed in entrypoint validation: %\" PRIu64 \"\n", ret);
    return ret;
  }

  if (!is_valid) {
    log_error(RANDOM_WALKER_ID,
              "Failed in entrypoint validation, entry point is not valid");
    return RC_CONSENSUS_EXIT_PROBABILITIES_INVALID_ENTRYPOINT;
  }

  memcpy(curr_tail_hash, ep->trits, ep->num_bytes);
  num_traversed_tails++;

  do {
    ret = random_walker_select_approver_tail(
        exit_probability_randomizer, curr_tail_hash, cw_result, ep_validator,
        &next_approver_tail, &has_next_approver_tail);
    if (ret) {
      return ret;
    } else if (has_next_approver_tail) {
      memcpy(curr_tail_hash, next_approver_tail.trits,
             next_approver_tail.num_bytes);
      num_traversed_tails++;
    }
  } while (has_next_approver_tail);

  memcpy(tip->trits, curr_tail_hash, FLEX_TRIT_SIZE_243);
  //(traversed_tails, tmp_element, num_traversed_tails);
  log_debug(RANDOM_WALKER_ID,
            "Number of tails traversed to find tip: \"%\" PRIu64 \"",
            num_traversed_tails);

  return ret;
}

retcode_t random_walker_select_approver_tail(
    const ep_randomizer_t *exit_probability_randomizer, flex_trit_t *curr_tail,
    cw_calc_result *cw_result, const exit_prob_transaction_validator_t *epv,
    trit_array_t *approver_tail_hash, bool *found_approver_tail) {
  retcode_t ret = RC_OK;
  bool has_next_approver;
  hash_to_direct_approvers_entry_t *curr_approver_entry = NULL;
  hash_entry_t *curr_approver = NULL;
  trit_array_t approver;
  flex_trit_t approver_trits[FLEX_TRIT_SIZE_243];
  approver.trits = (flex_trit_t *)&approver_trits;

  memcpy(approver.trits, curr_tail, FLEX_TRIT_SIZE_243);
  approver.num_bytes = FLEX_TRIT_SIZE_243;

  *found_approver_tail = false;
  HASH_FIND(hh, cw_result->tx_to_approvers, curr_tail, FLEX_TRIT_SIZE_243,
            curr_approver_entry);

  if (!curr_approver_entry) {
    *found_approver_tail = false;
    return RC_OK;
  }

  while (!(*found_approver_tail) &&
         HASH_COUNT(curr_approver_entry->approvers) > 0) {
    ret = select_approver(exit_probability_randomizer, cw_result->cw_ratings,
                          &curr_approver_entry->approvers, &approver,
                          &has_next_approver);
    if (ret != RC_OK) {
      *found_approver_tail = false;
      return ret;
    }
    if (has_next_approver) {
      ret = find_tail_if_valid(exit_probability_randomizer, &approver,
                               approver_tail_hash, found_approver_tail);
      if (ret != RC_OK) {
        return ret;
      }
      if (!(*found_approver_tail)) {
        HASH_FIND(hh, curr_approver_entry->approvers, approver.trits,
                  FLEX_TRIT_SIZE_243, curr_approver);
        HASH_DEL(curr_approver_entry->approvers, curr_approver);
        // if next tail is not valid, re-select while removing it from approvers
        // set
      }
    }
  }
  return RC_OK;
}

retcode_t select_approver(const ep_randomizer_t *exit_probability_randomizer,
                          cw_map_t cw_ratings, hash_set_t *approvers,
                          trit_array_t *approver, bool *has_next_approver) {
  hash_entry_t *curr_approver = NULL;
  hash_entry_t *tmp_approver = NULL;
  cw_entry_t *curr_rating = NULL;
  size_t num_approvers = HASH_COUNT(*approvers);
  int64_t weights[num_approvers];
  double sum_weights = 0;
  size_t idx = 0;

  *has_next_approver = false;

  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    HASH_FIND(hh, cw_ratings, curr_approver->hash, FLEX_TRIT_SIZE_243,
              curr_rating);
    weights[idx++] = curr_rating->cw;
  }

  int64_t max_weight = 0;
  for (idx = 0; idx < num_approvers; ++idx) {
    if (weights[idx] > max_weight) {
      max_weight = weights[idx];
    }
  }

  for (idx = 0; idx < num_approvers; ++idx) {
    weights[idx] -= max_weight;
    weights[idx] = exp(weights[idx] * exit_probability_randomizer->alpha);
    sum_weights += weights[idx];
  }

  double target = ((double)rand() / (double)RAND_MAX) * sum_weights;
  idx = 0;
  while (target > 0 && idx < num_approvers) {
    target -= weights[idx++];
  }
  --idx;

  size_t iter_idx = 0;
  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    if (iter_idx++ == idx) {
      *has_next_approver = true;
      memcpy(approver->trits, curr_approver->hash, FLEX_TRIT_SIZE_243);
      approver->num_bytes = FLEX_TRIT_SIZE_243;
      break;
    }
  }

  return RC_OK;
}

retcode_t find_tail_if_valid(const ep_randomizer_t *exit_probability_randomizer,
                             const trit_array_t *tx_hash,
                             trit_array_t *tail_hash, bool *found_tail) {
  retcode_t res;

  struct _iota_transaction curr_tx_s;
  struct _iota_transaction next_tx_s;
  iota_transaction_t curr_tx = &curr_tx_s;
  iota_transaction_t next_tx = &next_tx_s;
  trit_array_t curr_tx_hash;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  bool found_approver = false;
  *found_tail = false;

  iota_stor_pack_t tx_pack = {(void **)(&curr_tx), 1, 0, false};

  res = iota_tangle_transaction_load(exit_probability_randomizer->tangle,
                                     TRANSACTION_COL_HASH, tx_hash, &tx_pack);

  if (res != RC_OK || tx_pack.num_loaded == 0) {
    return res;
  }

  curr_tx = (iota_transaction_t)(tx_pack.models[0]);

  uint32_t index = curr_tx->current_index;
  memcpy(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243);

  iota_stor_pack_t hash_pack;
  if ((res = hash_pack_init(&hash_pack, 10)) != RC_OK) {
    return res;
  }

  while (res == RC_OK && index > 0 &&
         memcmp(curr_tx->bundle, bundle_hash, FLEX_TRIT_SIZE_243) == 0) {
    hash_pack.num_loaded = 0;
    hash_pack.insufficient_capacity = false;
    curr_tx_hash.num_bytes = FLEX_TRIT_SIZE_243;
    curr_tx_hash.trits = curr_tx->hash;
    res = iota_tangle_transaction_load_hashes_of_approvers(
        exit_probability_randomizer->tangle, &curr_tx_hash, &hash_pack);

    if (res != RC_OK) {
      log_error(CW_RATING_CALCULATOR_LOGGER_ID,
                "Failed in loading approvers, error code is: %\" PRIu64 \"\n",
                res);
      break;
    }

    --index;
    uint32_t approver_idx = 0;
    found_approver = false;
    while (approver_idx < hash_pack.num_loaded) {
      trit_array_p approver_hash =
          (trit_array_t *)hash_pack.models[approver_idx];
      tx_pack.capacity = 1;
      tx_pack.num_loaded = 0;
      tx_pack.insufficient_capacity = false;
      tx_pack.models = (void **)(&next_tx);
      res = iota_tangle_transaction_load(exit_probability_randomizer->tangle,
                                         TRANSACTION_COL_HASH, approver_hash,
                                         &tx_pack);
      if (res != RC_OK || tx_pack.num_loaded == 0) {
        break;
      }
      if (next_tx->current_index == index &&
          memcmp(next_tx->hash, bundle_hash, FLEX_TRIT_SIZE_243) == 0) {
        curr_tx = next_tx;
        found_approver = true;
        break;
      }
    }

    if (!found_approver) {
      break;
    }
  }

  if (curr_tx->current_index == 0) {
    memcpy(tail_hash->trits, curr_tx->hash, FLEX_TRIT_SIZE_243);
    tail_hash->num_bytes = FLEX_TRIT_SIZE_243;
    *found_tail = true;
  }

  hash_pack_free(&hash_pack);

  return res;
}
