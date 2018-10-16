/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "common/model/milestone.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/snapshot/snapshot.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

/*
 * Private functions
 */

static retcode_t update_snapshot_milestone(ledger_validator_t *const lv,
                                           flex_trit_t *const milestone_hash,
                                           uint64_t index) {
  retcode_t ret = RC_OK;
  hash_stack_t non_analyzed_hashes = NULL;
  hash_set_t analyzed_hashes = NULL;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  struct _trit_array tx_hash = {NULL, NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  if ((ret = hash_stack_push(&non_analyzed_hashes, milestone_hash)) != RC_OK) {
    goto done;
  }

  while (non_analyzed_hashes != NULL) {
    tx_hash.trits = hash_stack_peek(non_analyzed_hashes);
    if (!hash_set_contains(&analyzed_hashes, tx_hash.trits)) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load(
               lv->tangle, TRANSACTION_FIELD_HASH, &tx_hash, &pack)) != RC_OK) {
        goto done;
      }
      if (tx.snapshot_index == 0) {
        if ((ret = iota_tangle_transaction_update_snapshot_index(
                 lv->tangle, tx.hash, index)) != RC_OK) {
          goto done;
        }
        // TODO messageQ publish
        if ((ret = hash_stack_push(&non_analyzed_hashes, tx.trunk)) != RC_OK) {
          goto done;
        }
        if ((ret = hash_stack_push(&non_analyzed_hashes, tx.branch)) != RC_OK) {
          goto done;
        }
      }
      if ((ret = hash_set_add(&analyzed_hashes, tx_hash.trits)) != RC_OK) {
        goto done;
      }
    }
    hash_stack_pop(&non_analyzed_hashes);
  }

done:
  hash_stack_free(&non_analyzed_hashes);
  hash_set_free(&analyzed_hashes);
  return ret;
}

static retcode_t build_snapshot(ledger_validator_t *const lv,
                                uint64_t *const consistent_index,
                                flex_trit_t *const consistent_hash) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  if ((ret = iota_tangle_milestone_load_first(lv->tangle, &pack)) != RC_OK) {
    goto done;
  }

  while (pack.num_loaded != 0) {
    if (milestone.index % 10000 == 0) {
      log_info(LEDGER_VALIDATOR_LOGGER_ID,
               "Building snapshot... Consistent: #%" PRIu64
               ", Candidate: #%" PRIu64 "\n",
               *consistent_index, milestone.index);
    }
    // TODO state diff model
    //       StateDiffViewModel stateDiffViewModel =
    //           StateDiffViewModel.load(tangle, candidateMilestone.getHash());
    //
    //       if (stateDiffViewModel != null && !stateDiffViewModel.isEmpty()) {
    //         if
    //         (Snapshot.isConsistent(milestoneTracker.latestSnapshot.patchedDiff(
    //                 stateDiffViewModel.getDiff()))) {
    //           milestoneTracker.latestSnapshot.apply(stateDiffViewModel.getDiff(),
    //                                                 candidateMilestone.index());
    *consistent_index = milestone.index;
    memcpy(consistent_hash, milestone.hash, FLEX_TRIT_SIZE_243);
    //         } else {
    //           break;
    //         }
    //       }
    hash_pack_reset(&pack);
    if ((ret = iota_tangle_milestone_load_next(lv->tangle, milestone.index,
                                               &pack)) != RC_OK) {
      goto done;
    }
  }

done:
  return ret;
}

static retcode_t get_latest_diff(ledger_validator_t *const lv,
                                 hash_set_t *const analyzed_hashes,
                                 state_diff_t *state, flex_trit_t *const tip,
                                 uint64_t latest_snapshot_index,
                                 bool is_milestone, bool *valid_diff) {
  retcode_t ret = RC_OK;
  int number_of_analyzed_transactions = 0;
  hash_stack_t non_analyzed_hashes = NULL;
  state_diff_entry_t *entry, *diff_elem;
  iota_transaction_t tx_bundle = NULL;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array tx_hash = {NULL, NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  bool valid_bundle = false;
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  if ((ret = hash_stack_push(&non_analyzed_hashes, tip)) != RC_OK) {
    goto done;
  }

  {
    flex_trit_t null_hash[FLEX_TRIT_SIZE_243];
    memset(null_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    if ((ret = hash_set_add(analyzed_hashes, null_hash)) != RC_OK) {
      goto done;
    }
  }

  while (non_analyzed_hashes != NULL) {
    tx_hash.trits = hash_stack_peek(non_analyzed_hashes);
    if (!hash_set_contains(analyzed_hashes, tx_hash.trits)) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load(
               lv->tangle, TRANSACTION_FIELD_HASH, &tx_hash, &pack)) != RC_OK) {
        goto done;
      }
      if (pack.num_loaded == 0) {
        // TODO request transaction
        *valid_diff = false;
        goto done;
      }
      if (tx.snapshot_index == 0 || tx.snapshot_index > latest_snapshot_index) {
        number_of_analyzed_transactions++;
        if (tx.current_index == 0) {
          valid_bundle = false;
          if ((ret = iota_consensus_bundle_validator_validate(
                   lv->tangle, &tx_hash, bundle, &valid_bundle)) != RC_OK) {
            goto done;
          }
          if (valid_bundle && (tx_bundle = (iota_transaction_t)utarray_eltptr(
                                   bundle, 0)) != NULL) {
            while (tx_bundle != NULL) {
              if (tx_bundle->value != 0) {
                // TODO countedTx.add(bundleTransactionViewModel.getHash()))
                HASH_FIND(hh, *state, tx_bundle->address, FLEX_TRIT_SIZE_243,
                          entry);
                if (entry) {
                  entry->value += tx_bundle->value;
                } else {
                  if ((diff_elem = malloc(sizeof(state_diff_entry_t))) ==
                      NULL) {
                    ret = RC_LEDGER_VALIDATOR_OOM;
                    goto done;
                  }
                  memcpy(diff_elem->hash, tx_bundle->address,
                         FLEX_TRIT_SIZE_243);
                  diff_elem->value = tx_bundle->value;
                  HASH_ADD(hh, *state, hash, FLEX_TRIT_SIZE_243, diff_elem);
                }
              }
              tx_bundle = (iota_transaction_t)utarray_next(bundle, tx_bundle);
            }
          }
          if (!valid_bundle) {
            *valid_diff = false;
            goto done;
          }
        }
        if ((ret = hash_stack_push(&non_analyzed_hashes, tx.trunk)) != RC_OK) {
          goto done;
        }
        if ((ret = hash_stack_push(&non_analyzed_hashes, tx.branch)) != RC_OK) {
          goto done;
        }
      }
      if ((ret = hash_set_add(analyzed_hashes, tx_hash.trits)) != RC_OK) {
        goto done;
      }
    }
    hash_stack_pop(&non_analyzed_hashes);
  }
done:
  bundle_transactions_free(&bundle);
  hash_stack_free(&non_analyzed_hashes);
  hash_set_free(analyzed_hashes);
  return ret;
}

/*
 * Public functions
 */

retcode_t iota_consensus_ledger_validator_init(ledger_validator_t *const lv,
                                               tangle_t *const tangle,
                                               milestone_tracker_t *const mt,
                                               requester_state_t *const tr) {
  retcode_t ret = RC_OK;

  logger_helper_init(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  lv->tangle = tangle;
  lv->milestone_tracker = mt;
  lv->transaction_requester = tr;

  if ((ret = build_snapshot(lv, &mt->latest_solid_subtangle_milestone_index,
                            mt->latest_solid_subtangle_milestone->trits)) !=
      RC_OK) {
    log_critical(LEDGER_VALIDATOR_LOGGER_ID, "Building snapshot failed\n");
    return ret;
  }

  log_info(LEDGER_VALIDATOR_LOGGER_ID,
           "Loaded consistent milestone: #%" PRIu64 "\n",
           mt->latest_solid_subtangle_milestone_index);
  return ret;
}

retcode_t iota_consensus_ledger_validator_destroy(
    ledger_validator_t *const lv) {
  logger_helper_destroy(LEDGER_VALIDATOR_LOGGER_ID);
  lv->tangle = NULL;
  lv->milestone_tracker = NULL;
  lv->transaction_requester = NULL;
  return RC_OK;
}

retcode_t iota_consensus_ledger_validator_update_snapshot(
    ledger_validator_t *const lv, iota_milestone_t *const milestone,
    bool *const has_snapshot) {
  retcode_t ret = RC_OK;
  bool valid_diff = true;
  hash_set_t analyzed_hashes = NULL;
  state_diff_t diff = NULL, patch = NULL;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array milestone_hash = {milestone->hash, NUM_TRITS_HASH,
                                       FLEX_TRIT_SIZE_243, 0};

  rw_lock_handle_wrlock(&lv->milestone_tracker->latest_snapshot->rw_lock);

  if ((ret = iota_tangle_transaction_load(lv->tangle, TRANSACTION_FIELD_HASH,
                                          &milestone_hash, &pack)) != RC_OK) {
    goto done;
  } else if (pack.num_loaded != 1) {
    ret = RC_LEDGER_VALIDATOR_INVALID_MILESTONE;
    goto done;
  }
  *has_snapshot = tx.snapshot_index != 0;
  if (!(*has_snapshot)) {
    if ((ret = get_latest_diff(
             lv, &analyzed_hashes, &diff, tx.hash,
             iota_snapshot_get_index(lv->milestone_tracker->latest_snapshot),
             true, &valid_diff)) != RC_OK) {
      log_error(LEDGER_VALIDATOR_LOGGER_ID, "Getting latest diff failed\n");
      goto done;
    }
    if (!valid_diff) {
      *has_snapshot = false;
      goto done;
    }
    if ((ret = iota_snapshot_create_patch(
             lv->milestone_tracker->latest_snapshot, &diff, &patch)) != RC_OK) {
      log_error(LEDGER_VALIDATOR_LOGGER_ID, "Creating patch failed\n");
      goto done;
    }
    *has_snapshot = iota_snapshot_is_state_consistent(&patch);
    if (*has_snapshot) {
      if ((ret = update_snapshot_milestone(lv, milestone->hash,
                                           milestone->index)) != RC_OK) {
        log_error(LEDGER_VALIDATOR_LOGGER_ID,
                  "Updating snapshot milestone failed\n");
        goto done;
      }
      if (HASH_COUNT(diff) != 0) {
        //         stateDiffViewModel.store(tangle); // TODO
      }
      if ((ret =
               iota_snapshot_apply_patch(lv->milestone_tracker->latest_snapshot,
                                         &diff, milestone->index)) != RC_OK) {
        log_error(LEDGER_VALIDATOR_LOGGER_ID, "Applying patch failed\n");
        goto done;
      }
    }
  }
done:
  rw_lock_handle_unlock(&lv->milestone_tracker->latest_snapshot->rw_lock);
  if (diff) {
    iota_snapshot_state_destroy(&diff);
  }
  if (patch) {
    iota_snapshot_state_destroy(&patch);
  }
  return ret;
}

retcode_t iota_consensus_ledger_validator_check_consistency(
    ledger_validator_t *const lv, hash_stack_t hashes, bool *consistent) {
  retcode_t ret = RC_OK;
  hash_list_entry_t *iter = NULL;
  hash_set_t analyzed_hashes = NULL;
  state_diff_t diff = NULL;

  LL_FOREACH(hashes, iter) {
    if ((ret = iota_consensus_ledger_validator_update_diff(
             lv, &analyzed_hashes, &diff, iter->hash, consistent)) != RC_OK) {
      return ret;
    } else if (*consistent == false) {
      return ret;
    }
  }
  return ret;
}

retcode_t iota_consensus_ledger_validator_update_diff(
    ledger_validator_t *const lv, hash_set_t *analyzed_hashes,
    state_diff_t *diff, flex_trit_t *tip, bool *is_consistent) {
  retcode_t ret = RC_OK;
  state_diff_t current_state = NULL, patch = NULL;
  state_diff_entry_t *iter, *entry, *diff_elem, *tmp;
  hash_set_t visited_hashes = NULL;
  bool valid_diff = true;

  // TODO
  // if (!TransactionViewModel.fromHash(tangle, tip).isSolid()) {
  //   return false;
  // }

  if (hash_set_contains(analyzed_hashes, tip)) {
    *is_consistent = true;
    goto done;
  }

  if ((ret = hash_set_append(analyzed_hashes, &visited_hashes))) {
    goto done;
  }

  if ((ret = get_latest_diff(
           lv, &visited_hashes, &current_state, tip,
           iota_snapshot_get_index(lv->milestone_tracker->latest_snapshot),
           false, &valid_diff)) != RC_OK) {
    log_error(LEDGER_VALIDATOR_LOGGER_ID, "Getting latest diff failed\n");
    goto done;
  }

  if (!valid_diff) {
    *is_consistent = false;
    goto done;
  }

  // TODO Extract state_diff_t from snapshot #345
  HASH_ITER(hh, *diff, iter, tmp) {
    HASH_FIND(hh, current_state, iter->hash, FLEX_TRIT_SIZE_243, entry);
    if (entry) {
      entry->value += iter->value;
    } else {
      if ((diff_elem = malloc(sizeof(state_diff_entry_t))) == NULL) {
        ret = RC_LEDGER_VALIDATOR_OOM;
        goto done;
      }
      memcpy(diff_elem->hash, iter->hash, FLEX_TRIT_SIZE_243);
      diff_elem->value = iter->value;
      HASH_ADD(hh, current_state, hash, FLEX_TRIT_SIZE_243, diff_elem);
    }
  }

  if ((ret = iota_snapshot_create_patch(lv->milestone_tracker->latest_snapshot,
                                        &current_state, &patch)) != RC_OK) {
    log_error(LEDGER_VALIDATOR_LOGGER_ID, "Creating patch failed\n");
    goto done;
  }
  *is_consistent = iota_snapshot_is_state_consistent(&patch);

  if (is_consistent) {
    // TODO Extract state_diff_t from snapshot #345
    HASH_ITER(hh, current_state, iter, tmp) {
      HASH_FIND(hh, *diff, iter->hash, FLEX_TRIT_SIZE_243, entry);
      if (entry) {
        entry->value = iter->value;
      } else {
        if ((diff_elem = malloc(sizeof(state_diff_entry_t))) == NULL) {
          ret = RC_LEDGER_VALIDATOR_OOM;
          goto done;
        }
        memcpy(diff_elem->hash, iter->hash, FLEX_TRIT_SIZE_243);
        diff_elem->value = iter->value;
        HASH_ADD(hh, *diff, hash, FLEX_TRIT_SIZE_243, diff_elem);
      }
    }
    if ((ret = hash_set_append(&visited_hashes, analyzed_hashes))) {
      goto done;
    }
  }

done:
  return ret;
}
