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
#include "consensus/utils/tangle_traversals.h"
#include "gossip/components/transaction_requester.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

/*
 * Private functions
 */

static retcode_t update_snapshot_milestone_do_func(flex_trit_t *const hash,
                                                   iota_stor_pack_t *pack,
                                                   void *data,
                                                   bool *should_branch,
                                                   bool *should_stop) {
  retcode_t ret;
  hash243_set_t *hashes = data;

  if (pack->num_loaded == 0) {
    *should_stop = true;
    log_error(LEDGER_VALIDATOR_LOGGER_ID, "In %s, could not load milestone\n",
              __FUNCTION__);
    return RC_LEDGER_VALIDATOR_COULD_NOT_LOAD_MILESTONE;
  }
  iota_transaction_t transaction = pack->models[0];
  *should_branch = transaction->snapshot_index == 0;

  if (*should_branch) {
    if ((ret = hash243_set_add(hashes, transaction->hash)) != RC_OK) {
      *should_stop = true;
      return ret;
    }
  }

  return RC_OK;
}

static retcode_t update_snapshot_milestone(ledger_validator_t const *const lv,
                                           flex_trit_t *const hash,
                                           uint64_t index) {
  retcode_t ret;
  hash243_set_t hashes_to_update = NULL;

  if ((ret = tangle_traversal_dfs_to_genesis(
           lv->tangle, update_snapshot_milestone_do_func, hash,
           lv->conf->genesis_hash, NULL, &hashes_to_update)) != RC_OK) {
    return ret;
  }
  ret = iota_tangle_transactions_update_snapshot_index(lv->tangle,
                                                       hashes_to_update, index);
  hash243_set_free(&hashes_to_update);
  return ret;
}

static retcode_t build_snapshot(ledger_validator_t const *const lv,
                                uint64_t *const consistent_index,
                                flex_trit_t *const consistent_hash) {
  retcode_t ret = RC_OK;
  state_delta_t delta = NULL;
  state_delta_t patch = NULL;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  rw_lock_handle_wrlock(&lv->milestone_tracker->latest_snapshot->rw_lock);

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
    if ((ret = iota_tangle_state_delta_load(lv->tangle, milestone.index,
                                            &delta)) != RC_OK) {
      goto done;
    }
    if (delta != NULL && !state_delta_empty(delta)) {
      if ((ret = iota_snapshot_create_patch(
               lv->milestone_tracker->latest_snapshot, &delta, &patch)) !=
          RC_OK) {
        goto done;
      }
      if (state_delta_is_consistent(&patch)) {
        if ((ret = iota_snapshot_apply_patch(
                 lv->milestone_tracker->latest_snapshot, &delta,
                 milestone.index)) != RC_OK) {
          goto done;
        }
        *consistent_index = milestone.index;
        memcpy(consistent_hash, milestone.hash, FLEX_TRIT_SIZE_243);
      } else {
        log_critical(LEDGER_VALIDATOR_LOGGER_ID,
                     "Inconsistent state delta loaded\n");
        ret = RC_LEDGER_VALIDATOR_INCONSISTENT_DELTA;
        goto done;
      }
    }
    hash_pack_reset(&pack);
    if ((ret = iota_tangle_milestone_load_next(lv->tangle, milestone.index,
                                               &pack)) != RC_OK) {
      goto done;
    }
    state_delta_destroy(&delta);
    state_delta_destroy(&patch);
  }

done:
  rw_lock_handle_unlock(&lv->milestone_tracker->latest_snapshot->rw_lock);
  state_delta_destroy(&delta);
  state_delta_destroy(&patch);
  return ret;
}

typedef struct get_latest_delta_do_func_params_s {
  ledger_validator_t *lv;
  bool valid_delta;
  bool is_milestone;
  uint64_t latest_snapshot_index;
  state_delta_t *state;
} get_latest_delta_do_func_params_t;

static retcode_t get_latest_delta_do_func(flex_trit_t *hash,
                                          iota_stor_pack_t *pack, void *data,
                                          bool *should_branch,
                                          bool *should_stop) {
  retcode_t ret = RC_OK;
  bool valid_bundle = false;
  struct _trit_array curr_hash = {.trits = NULL,
                                  .num_trits = NUM_TRITS_HASH,
                                  .num_bytes = FLEX_TRIT_SIZE_243,
                                  .dynamic = 0};
  bundle_transactions_t *bundle = NULL;
  iota_transaction_t tx_bundle = NULL;

  *should_stop = false;
  *should_branch = false;
  get_latest_delta_do_func_params_t *params =
      (get_latest_delta_do_func_params_t *)data;
  ledger_validator_t *lv = params->lv;

  // Transaction is not marked solid, but it is a candidate
  if (pack->num_loaded == 0) {
    if (lv->transaction_requester) {
      ret = request_transaction(lv->transaction_requester, hash,
                                params->is_milestone);
    }
    params->valid_delta = false;
    *should_stop = true;
    return ret;
  }

  iota_transaction_t tx = pack->models[0];
  *should_branch = true;
  if (tx->snapshot_index == 0 ||
      tx->snapshot_index > params->latest_snapshot_index) {
    if (tx->current_index == 0) {
      bundle_transactions_new(&bundle);
      curr_hash.trits = hash;
      if ((ret = iota_consensus_bundle_validator_validate(
               lv->tangle, &curr_hash, bundle, &valid_bundle)) != RC_OK) {
        goto done;
      }
      if (!valid_bundle ||
          (tx_bundle = (iota_transaction_t)utarray_eltptr(bundle, 0)) == NULL) {
        params->valid_delta = false;
        *should_stop = true;
        goto done;
      }
      while (tx_bundle != NULL) {
        if (tx_bundle->value != 0) {
          if ((ret = state_delta_add_or_sum(params->state, tx_bundle->address,
                                            tx_bundle->value)) != RC_OK) {
            goto done;
          }
        }
        tx_bundle = (iota_transaction_t)utarray_next(bundle, tx_bundle);
      }
    }
  }

done:
  bundle_transactions_free(&bundle);
  if (ret != RC_OK) {
    *should_stop = true;
    params->valid_delta = false;
  }
  return ret;
}

static retcode_t get_latest_delta(ledger_validator_t const *const lv,
                                  hash243_set_t *const analyzed_hashes,
                                  state_delta_t *const state,
                                  flex_trit_t const *const tip,
                                  uint64_t const latest_snapshot_index,
                                  bool const is_milestone,
                                  bool *const valid_delta) {
  retcode_t ret = RC_OK;

  get_latest_delta_do_func_params_t params = {
      .state = state,
      .valid_delta = true,
      .latest_snapshot_index = latest_snapshot_index,
      .is_milestone = is_milestone,
      .lv = lv};

  ret = tangle_traversal_dfs_to_genesis(lv->tangle, get_latest_delta_do_func,
                                        tip, lv->conf->genesis_hash,
                                        analyzed_hashes, (void *)&params);
  *valid_delta = params.valid_delta;
  return ret;
}

/*
 * Public functions
 */

retcode_t iota_consensus_ledger_validator_init(
    ledger_validator_t *const lv, iota_consensus_conf_t *const conf,
    tangle_t *const tangle, milestone_tracker_t *const mt,
    requester_state_t *const tr) {
  retcode_t ret = RC_OK;

  logger_helper_init(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  lv->conf = conf;
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
  lv->tangle = NULL;
  lv->milestone_tracker = NULL;
  lv->transaction_requester = NULL;
  logger_helper_destroy(LEDGER_VALIDATOR_LOGGER_ID);
  return RC_OK;
}

retcode_t iota_consensus_ledger_validator_update_snapshot(
    ledger_validator_t const *const lv, iota_milestone_t *const milestone,
    bool *const has_snapshot) {
  retcode_t ret = RC_OK;
  bool valid_delta = true;
  hash243_set_t analyzed_hashes = NULL;
  state_delta_t delta = NULL;
  state_delta_t patch = NULL;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array milestone_hash = {milestone->hash, NUM_TRITS_HASH,
                                       FLEX_TRIT_SIZE_243, 0};

  *has_snapshot = false;
  rw_lock_handle_wrlock(&lv->milestone_tracker->latest_snapshot->rw_lock);

  if ((ret = iota_tangle_transaction_load(lv->tangle, TRANSACTION_FIELD_HASH,
                                          &milestone_hash, &pack)) != RC_OK) {
    goto done;
  } else if (pack.num_loaded == 0) {
    ret = RC_LEDGER_VALIDATOR_INVALID_MILESTONE;
    goto done;
  }

  *has_snapshot = tx.snapshot_index != 0;
  if (!(*has_snapshot)) {
    if ((ret = get_latest_delta(
             lv, &analyzed_hashes, &delta, tx.hash,
             iota_snapshot_get_index(lv->milestone_tracker->latest_snapshot),
             true, &valid_delta)) != RC_OK) {
      log_error(LEDGER_VALIDATOR_LOGGER_ID, "Getting latest delta failed\n");
      goto done;
    } else if (!valid_delta) {
      *has_snapshot = false;
      goto done;
    }
    if ((ret = iota_snapshot_create_patch(
             lv->milestone_tracker->latest_snapshot, &delta, &patch)) !=
        RC_OK) {
      log_error(LEDGER_VALIDATOR_LOGGER_ID, "Creating patch failed\n");
      goto done;
    }
    if ((*has_snapshot = state_delta_is_consistent(&patch))) {
      if ((ret = update_snapshot_milestone(lv, milestone->hash,
                                           milestone->index)) != RC_OK) {
        log_error(LEDGER_VALIDATOR_LOGGER_ID,
                  "Updating snapshot milestone failed\n");
        goto done;
      }
      if (!state_delta_empty(delta)) {
        if ((ret = iota_tangle_state_delta_store(lv->tangle, milestone->index,
                                                 &delta)) != RC_OK) {
          goto done;
        }
      }
      if ((ret =
               iota_snapshot_apply_patch(lv->milestone_tracker->latest_snapshot,
                                         &delta, milestone->index)) != RC_OK) {
        log_error(LEDGER_VALIDATOR_LOGGER_ID, "Applying patch failed\n");
        goto done;
      }
    }
  }

done:
  rw_lock_handle_unlock(&lv->milestone_tracker->latest_snapshot->rw_lock);
  state_delta_destroy(&delta);
  state_delta_destroy(&patch);
  return ret;
}

retcode_t iota_consensus_ledger_validator_check_consistency(
    ledger_validator_t const *const lv, hash243_stack_t const hashes,
    bool *const is_consistent) {
  retcode_t ret = RC_OK;
  hash243_stack_entry_t *iter = NULL;
  hash243_set_t analyzed_hashes = NULL;
  state_delta_t delta = NULL;

  *is_consistent = true;
  LL_FOREACH(hashes, iter) {
    if ((ret = iota_consensus_ledger_validator_update_delta(
             lv, &analyzed_hashes, &delta, iter->hash, is_consistent)) !=
        RC_OK) {
      *is_consistent = false;
      goto done;
    } else if (*is_consistent == false) {
      goto done;
    }
  }

done:
  hash243_set_free(&analyzed_hashes);
  state_delta_destroy(&delta);
  return ret;
}

retcode_t iota_consensus_ledger_validator_update_delta(
    ledger_validator_t const *const lv, hash243_set_t *const analyzed_hashes,
    state_delta_t *const delta, flex_trit_t const *const tip,
    bool *const is_consistent) {
  retcode_t ret = RC_OK;
  state_delta_t tip_state = NULL;
  state_delta_t patch = NULL;
  hash243_set_t visited_hashes = NULL;
  bool valid_delta = true;

  *is_consistent = false;

  // TODO Waiting for Transaction solidifier - WIP #405

  if (hash243_set_contains(analyzed_hashes, tip)) {
    *is_consistent = true;
    goto done;
  }

  if ((ret = hash243_set_append(analyzed_hashes, &visited_hashes)) != RC_OK) {
    goto done;
  }

  if ((ret = get_latest_delta(
           lv, &visited_hashes, &tip_state, tip,
           iota_snapshot_get_index(lv->milestone_tracker->latest_snapshot),
           false, &valid_delta)) != RC_OK) {
    log_error(LEDGER_VALIDATOR_LOGGER_ID, "Getting latest delta failed\n");
    goto done;
  }

  if (!valid_delta) {
    goto done;
  }

  if ((ret = state_delta_apply_patch(&tip_state, delta)) != RC_OK) {
    log_error(LEDGER_VALIDATOR_LOGGER_ID, "Applying patch failed\n");
    goto done;
  }

  if ((ret = iota_snapshot_create_patch(lv->milestone_tracker->latest_snapshot,
                                        &tip_state, &patch)) != RC_OK) {
    log_error(LEDGER_VALIDATOR_LOGGER_ID, "Creating patch failed\n");
    goto done;
  }

  if (state_delta_is_consistent(&patch)) {
    if ((ret = state_delta_merge_patch(delta, &tip_state)) != RC_OK) {
      log_error(LEDGER_VALIDATOR_LOGGER_ID, "Merging patch failed\n");
      goto done;
    }
    if ((ret = hash243_set_append(&visited_hashes, analyzed_hashes)) != RC_OK) {
      goto done;
    }
    *is_consistent = true;
  }

done:
  state_delta_destroy(&tip_state);
  state_delta_destroy(&patch);
  hash243_set_free(&visited_hashes);
  return ret;
}
