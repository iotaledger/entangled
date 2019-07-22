/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdlib.h>

#include "ciri/consensus/bundle_validator/bundle_validator.h"
#include "ciri/consensus/ledger_validator/ledger_validator.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/snapshot/snapshot.h"
#include "ciri/consensus/tangle/traversal.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "ledger_validator"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t update_snapshot_milestone_do_func(flex_trit_t *const hash, iota_stor_pack_t *pack, void *data,
                                                   bool *should_branch, bool *should_stop) {
  retcode_t ret;
  hash243_set_t *hashes = data;

  if (pack->num_loaded == 0) {
    *should_stop = true;
    log_error(logger_id, "In %s, could not load milestone\n", __FUNCTION__);
    return RC_LEDGER_VALIDATOR_COULD_NOT_LOAD_MILESTONE;
  }
  iota_transaction_t *transaction = pack->models[0];
  *should_branch = transaction_snapshot_index(transaction) == 0;

  if (*should_branch) {
    if ((ret = hash243_set_add(hashes, hash)) != RC_OK) {
      *should_stop = true;
      return ret;
    }
  }

  return RC_OK;
}

static retcode_t update_snapshot_milestone(ledger_validator_t const *const lv, tangle_t const *const tangle,
                                           flex_trit_t *const hash, uint64_t index) {
  retcode_t ret = RC_OK;
  hash243_set_t hashes_to_update = NULL;
  hash243_set_t analyzed_hashes = NULL;

  // TODO this could be removed if SEPs were in database instead of in a map
  iota_snapshot_solid_entry_points_set(&lv->milestone_tracker->snapshots_provider->inital_snapshot, &analyzed_hashes);

  if ((ret = tangle_traversal_dfs_to_past(tangle, update_snapshot_milestone_do_func, hash, lv->conf->genesis_hash,
                                          &analyzed_hashes, &hashes_to_update)) != RC_OK) {
    return ret;
  }

  ret = iota_tangle_transactions_update_snapshot_index(tangle, hashes_to_update, index);

  hash243_set_free(&hashes_to_update);
  hash243_set_free(&analyzed_hashes);

  return ret;
}

static retcode_t build_snapshot(ledger_validator_t const *const lv, tangle_t const *const tangle,
                                uint64_t *const consistent_index, flex_trit_t *const consistent_hash) {
  retcode_t ret = RC_OK;
  state_delta_t delta = NULL;
  state_delta_t patch = NULL;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  if ((ret = iota_tangle_milestone_load_by_index(
           tangle, lv->milestone_tracker->snapshots_provider->inital_snapshot.metadata.index + 1, &pack)) != RC_OK) {
    goto done;
  }

  while (pack.num_loaded != 0) {
    if (milestone.index % 10000 == 0) {
      log_info(logger_id, "Building snapshot... Consistent: #%" PRIu64 ", Candidate: #%" PRIu64 "\n", *consistent_index,
               milestone.index);
    }
    if ((ret = iota_tangle_state_delta_load(tangle, milestone.index, &delta)) != RC_OK) {
      goto done;
    }
    if (delta != NULL && !state_delta_empty(delta)) {
      if ((ret = iota_snapshot_create_patch(&lv->milestone_tracker->snapshots_provider->latest_snapshot, &delta,
                                            &patch)) != RC_OK) {
        goto done;
      }
      if (state_delta_is_consistent(&patch)) {
        if ((ret = iota_snapshot_apply_patch(&lv->milestone_tracker->snapshots_provider->latest_snapshot, &delta,
                                             milestone.index)) != RC_OK) {
          goto done;
        }
        *consistent_index = milestone.index;
        memcpy(consistent_hash, milestone.hash, FLEX_TRIT_SIZE_243);
      } else {
        log_critical(logger_id, "Inconsistent state delta loaded\n");
        ret = RC_LEDGER_VALIDATOR_INCONSISTENT_DELTA;
        goto done;
      }
    }
    hash_pack_reset(&pack);
    if ((ret = iota_tangle_milestone_load_by_index(tangle, milestone.index + 1, &pack)) != RC_OK) {
      goto done;
    }
    state_delta_destroy(&delta);
    state_delta_destroy(&patch);
  }

done:
  state_delta_destroy(&delta);
  state_delta_destroy(&patch);
  return ret;
}

typedef struct get_latest_delta_do_func_params_s {
  tangle_t *tangle;
  bool valid_delta;
  bool is_milestone;
  uint64_t latest_snapshot_index;
  state_delta_t *state;
} get_latest_delta_do_func_params_t;

static retcode_t get_latest_delta_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data, bool *should_branch,
                                          bool *should_stop) {
  retcode_t ret = RC_OK;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  bundle_transactions_t *bundle = NULL;
  iota_transaction_t *tx_bundle = NULL;

  *should_stop = false;
  *should_branch = false;
  get_latest_delta_do_func_params_t *params = (get_latest_delta_do_func_params_t *)data;
  tangle_t *tangle = params->tangle;
  iota_transaction_t *tx = pack->models[0];

  if (transaction_snapshot_index(tx) == 0 || transaction_snapshot_index(tx) > params->latest_snapshot_index) {
    *should_branch = true;
    if (transaction_current_index(tx) == 0) {
      bundle_transactions_new(&bundle);
      if ((ret = iota_consensus_bundle_validator_validate(tangle, hash, bundle, &bundle_status)) != RC_OK) {
        goto done;
      }
      if (bundle_status != BUNDLE_VALID || (tx_bundle = (iota_transaction_t *)utarray_eltptr(bundle, 0)) == NULL) {
        params->valid_delta = false;
        *should_stop = true;
        goto done;
      }
      while (tx_bundle != NULL) {
        if ((ret = state_delta_add_or_sum(params->state, transaction_address(tx_bundle),
                                          transaction_value(tx_bundle))) != RC_OK) {
          goto done;
        }
        tx_bundle = (iota_transaction_t *)utarray_next(bundle, tx_bundle);
      }
      bundle_transactions_free(&bundle);
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

// This function should always be called with a solid entry point
static retcode_t get_latest_delta(ledger_validator_t const *const lv, tangle_t *const tangle,
                                  hash243_set_t *const analyzed_hashes, state_delta_t *const state,
                                  flex_trit_t const *const tip, uint64_t const latest_snapshot_index,
                                  bool const is_milestone, bool *const valid_delta) {
  retcode_t ret = RC_OK;
  get_latest_delta_do_func_params_t params = {.state = state,
                                              .valid_delta = true,
                                              .latest_snapshot_index = latest_snapshot_index,
                                              .is_milestone = is_milestone,
                                              .tangle = tangle};

  // TODO this could be removed if SEPs were in database instead of in a map
  iota_snapshot_solid_entry_points_set(&lv->milestone_tracker->snapshots_provider->inital_snapshot, analyzed_hashes);

  ret = tangle_traversal_dfs_to_past(tangle, get_latest_delta_do_func, tip, lv->conf->genesis_hash, analyzed_hashes,
                                     &params);
  *valid_delta = params.valid_delta;

  return ret;
}

/*
 * Public functions
 */

retcode_t iota_consensus_ledger_validator_init(ledger_validator_t *const lv, tangle_t const *const tangle,
                                               iota_consensus_conf_t *const conf, milestone_tracker_t *const mt) {
  retcode_t ret = RC_OK;

  logger_id = logger_helper_enable(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  lv->conf = conf;
  lv->milestone_tracker = mt;

  if ((ret = build_snapshot(lv, tangle, &mt->latest_solid_milestone_index, mt->latest_solid_milestone)) != RC_OK) {
    log_critical(logger_id, "Building snapshot failed\n");
    return ret;
  }

  return ret;
}

retcode_t iota_consensus_ledger_validator_destroy(ledger_validator_t *const lv) {
  lv->milestone_tracker = NULL;
  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_consensus_ledger_validator_update_snapshot(ledger_validator_t const *const lv, tangle_t *const tangle,
                                                          iota_milestone_t *const milestone, bool *const has_snapshot) {
  retcode_t ret = RC_OK;
  bool valid_delta = true;
  state_delta_t delta = NULL;
  state_delta_t patch = NULL;
  hash243_set_t analyzed_hashes = NULL;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  *has_snapshot = false;

  if ((ret = iota_tangle_transaction_load_partial(tangle, milestone->hash, &pack, PARTIAL_TX_MODEL_METADATA)) !=
      RC_OK) {
    goto done;
  } else if (pack.num_loaded == 0) {
    ret = RC_LEDGER_VALIDATOR_INVALID_TRANSACTION;
    goto done;
  } else if (transaction_solid(&tx) == 0) {
    ret = RC_LEDGER_VALIDATOR_TRANSACTION_NOT_SOLID;
    goto done;
  }

  *has_snapshot = transaction_snapshot_index(&tx) != 0;
  if (!(*has_snapshot)) {
    if ((ret = get_latest_delta(lv, tangle, &analyzed_hashes, &delta, milestone->hash,
                                iota_snapshot_get_index(&lv->milestone_tracker->snapshots_provider->latest_snapshot),
                                true, &valid_delta)) != RC_OK) {
      log_error(logger_id, "Getting latest delta failed\n");
      goto done;
    } else if (!valid_delta) {
      *has_snapshot = false;
      goto done;
    }
    if ((ret = iota_snapshot_create_patch(&lv->milestone_tracker->snapshots_provider->latest_snapshot, &delta,
                                          &patch)) != RC_OK) {
      log_error(logger_id, "Creating patch failed\n");
      goto done;
    }
    if ((*has_snapshot = state_delta_is_consistent(&patch))) {
      if ((ret = update_snapshot_milestone(lv, tangle, milestone->hash, milestone->index)) != RC_OK) {
        log_error(logger_id, "Updating snapshot milestone failed\n");
        goto done;
      }
      if (!state_delta_empty(delta)) {
        if ((ret = iota_tangle_state_delta_store(tangle, milestone->index, &delta)) != RC_OK) {
          goto done;
        }
      }

      if ((ret = iota_snapshot_apply_patch(&lv->milestone_tracker->snapshots_provider->latest_snapshot, &delta,
                                           milestone->index)) != RC_OK) {
        log_error(logger_id, "Applying patch failed\n");
        goto done;
      }
    }
  }

done:
  hash243_set_free(&analyzed_hashes);
  state_delta_destroy(&delta);
  state_delta_destroy(&patch);
  return ret;
}

retcode_t iota_consensus_ledger_validator_check_consistency(ledger_validator_t const *const lv, tangle_t *const tangle,
                                                            hash243_stack_t const hashes, bool *const is_consistent) {
  retcode_t ret = RC_OK;
  hash243_stack_entry_t *iter = NULL;
  hash243_set_t analyzed_hashes = NULL;
  state_delta_t delta = NULL;

  *is_consistent = true;
  LL_FOREACH(hashes, iter) {
    if ((ret = iota_consensus_ledger_validator_update_delta(lv, tangle, &analyzed_hashes, &delta, iter->hash,
                                                            is_consistent)) != RC_OK) {
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

retcode_t iota_consensus_ledger_validator_update_delta(ledger_validator_t const *const lv, tangle_t *const tangle,
                                                       hash243_set_t *const analyzed_hashes, state_delta_t *const delta,
                                                       flex_trit_t const *const tip, bool *const is_consistent) {
  retcode_t ret = RC_OK;
  state_delta_t tip_state = NULL;
  state_delta_t patch = NULL;
  hash243_set_t visited_hashes = NULL;
  bool valid_delta = true;

  *is_consistent = false;
  // Load the transaction
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  if ((ret = iota_tangle_transaction_load_partial(tangle, tip, &pack, PARTIAL_TX_MODEL_METADATA)) != RC_OK) {
    goto done;
  } else if (!transaction_solid(curr_tx)) {
    ret = RC_LEDGER_VALIDATOR_TRANSACTION_NOT_SOLID;
    goto done;
  }

  if (hash243_set_contains(*analyzed_hashes, tip)) {
    *is_consistent = true;
    goto done;
  }

  if ((ret = hash243_set_append(analyzed_hashes, &visited_hashes)) != RC_OK) {
    goto done;
  }

  if ((ret = get_latest_delta(lv, tangle, &visited_hashes, &tip_state, tip,
                              iota_snapshot_get_index(&lv->milestone_tracker->snapshots_provider->latest_snapshot),
                              false, &valid_delta)) != RC_OK) {
    log_error(logger_id, "Getting latest delta failed\n");
    goto done;
  }

  if (!valid_delta) {
    goto done;
  }

  if ((ret = state_delta_apply_patch(&tip_state, delta)) != RC_OK) {
    log_error(logger_id, "Applying patch failed\n");
    goto done;
  }

  if ((ret = iota_snapshot_create_patch(&lv->milestone_tracker->snapshots_provider->latest_snapshot, &tip_state,
                                        &patch)) != RC_OK) {
    log_error(logger_id, "Creating patch failed\n");
    goto done;
  }

  if (state_delta_is_consistent(&patch)) {
    if ((ret = state_delta_merge_patch(delta, &tip_state)) != RC_OK) {
      log_error(logger_id, "Merging patch failed\n");
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
