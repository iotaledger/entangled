/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <math.h>

#include "utlist.h"

#include "consensus/exit_probability_randomizer/walker.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define RANDOM_WALKER_LOGGER_ID "consensus_random_walker"

void init_ep_randomizer_walker(ep_randomizer_t *randomizer) {
  randomizer->base.vtable = random_walk_vtable;
}

/*
 * Private functions
 */

static retcode_t select_approver(
    const ep_randomizer_t *exit_probability_randomizer,
    hash_int_map_t cw_ratings, hash_set_t *approvers, trit_array_t *approver,
    bool *has_next_approver) {
  hash_set_entry_t *curr_approver = NULL;
  hash_set_entry_t *tmp_approver = NULL;
  hash_to_int_value_map_entry *curr_rating = NULL;
  size_t num_approvers = HASH_COUNT(*approvers);
  int64_t weights[num_approvers];
  double sum_weights = 0;
  double target = 0;
  int64_t max_weight = 0;
  size_t idx = 0;

  *has_next_approver = false;

  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    HASH_FIND(hh, cw_ratings, curr_approver->hash, FLEX_TRIT_SIZE_243,
              curr_rating);
    if (curr_rating == NULL) {
      log_error(RANDOM_WALKER_LOGGER_ID, "No rating found for approver\n");
      return RC_CONSENSUS_EXIT_PROBABILITIES_MISSING_RATING;
    }
    weights[idx++] = curr_rating->value;
    max_weight = MAX(max_weight, curr_rating->value);
  }

  for (idx = 0; idx < num_approvers; ++idx) {
    weights[idx] -= max_weight;
    weights[idx] = exp(weights[idx] * exit_probability_randomizer->alpha);
    sum_weights += weights[idx];
  }

  idx = 0;
  target = ((double)rand() / (double)RAND_MAX) * sum_weights;
  HASH_ITER(hh, *approvers, curr_approver, tmp_approver) {
    if ((target = (target - weights[idx++])) <= 0) {
      *has_next_approver = true;
      memcpy(approver->trits, curr_approver->hash, FLEX_TRIT_SIZE_243);
      break;
    }
  }

  return RC_OK;
}

static retcode_t find_tail_if_valid(
    const ep_randomizer_t *exit_probability_randomizer,
    const trit_array_t *tx_hash, trit_array_t *tail_hash, bool *found_tail) {
  retcode_t res;

  struct _iota_transaction next_tx_s;
  iota_transaction_t next_tx = &next_tx_s;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  bool found_approver = false;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, tx_pack);
  *found_tail = false;

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
    res = iota_tangle_transaction_load_hashes_of_approvers(
        exit_probability_randomizer->tangle, curr_tx->hash, &hash_pack);

    if (res != RC_OK) {
      log_error(RANDOM_WALKER_LOGGER_ID,
                "Failed in loading approvers, error code is: %" PRIu64 "\n",
                res);
      break;
    }

    --index;
    uint32_t approver_idx = 0;
    found_approver = false;
    while (approver_idx < hash_pack.num_loaded) {
      trit_array_p approver_hash =
          (trit_array_t *)hash_pack.models[approver_idx];
      tx_pack.models = (void **)(&next_tx);
      hash_pack_reset(&tx_pack);
      res = iota_tangle_transaction_load(exit_probability_randomizer->tangle,
                                         TRANSACTION_COL_HASH, approver_hash,
                                         &tx_pack);
      if (res != RC_OK || tx_pack.num_loaded == 0) {
        break;
      }
      if (next_tx->current_index == index &&
          memcmp(next_tx->bundle, bundle_hash, FLEX_TRIT_SIZE_243) == 0) {
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

static retcode_t random_walker_select_approver_tail(
    const ep_randomizer_t *exit_probability_randomizer,
    const exit_prob_transaction_validator_t *epv, flex_trit_t *curr_tail,
    cw_calc_result *cw_result, trit_array_t *approver_tail_hash,
    bool *found_approver_tail) {
  retcode_t ret = RC_OK;
  bool has_next_approver;
  hash_to_indexed_hash_set_entry_t *curr_approver_entry = NULL;
  hash_set_entry_t *curr_approver = NULL;
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
        // if next tail is not valid, re-select while removing it from
        // approvers set
      }
    }
  }
  return RC_OK;
}

/*
 * Public functions
 */

retcode_t iota_consensus_random_walker_randomize(
    const ep_randomizer_t *exit_probability_randomizer,
    const exit_prob_transaction_validator_t *ep_validator,
    cw_calc_result *cw_result, const trit_array_p ep, trit_array_p tip) {
  retcode_t ret = RC_OK;
  bool ep_is_valid = false;
  bool has_approver_tail = false;
  size_t num_traversed_tails = 1;
  flex_trit_t *curr_tail = ep->trits;
  flex_trit_t approver_tail_trits[FLEX_TRIT_SIZE_243];
  trit_array_t approver_tail = {.trits = approver_tail_trits,
                                .num_trits = HASH_LENGTH_TRIT,
                                .num_bytes = FLEX_TRIT_SIZE_243,
                                .dynamic = 0};

  ret = iota_consensus_exit_prob_transaction_validator_is_valid(
      ep_validator, ep, &ep_is_valid);
  if (ret != RC_OK) {
    log_error(RANDOM_WALKER_LOGGER_ID,
              "Entry point validation failed: %" PRIu64 "\n", ret);
    return ret;
  } else if (!ep_is_valid) {
    log_error(RANDOM_WALKER_LOGGER_ID, "Invalid entry point\n");
    return RC_CONSENSUS_EXIT_PROBABILITIES_INVALID_ENTRYPOINT;
  }

  do {
    ret = random_walker_select_approver_tail(
        exit_probability_randomizer, ep_validator, curr_tail, cw_result,
        &approver_tail, &has_approver_tail);
    if (ret != RC_OK) {
      log_error(RANDOM_WALKER_LOGGER_ID,
                "Selecting approver tail failed: %" PRIu64 "\n", ret);
      return ret;
    } else if (has_approver_tail) {
      curr_tail = approver_tail_trits;
      num_traversed_tails++;
    }
  } while (has_approver_tail);

  memcpy(tip->trits, curr_tail, FLEX_TRIT_SIZE_243);
  log_debug(RANDOM_WALKER_LOGGER_ID,
            "Number of tails traversed to find tip: %" PRIu64 "\n",
            num_traversed_tails);

  return ret;
}
