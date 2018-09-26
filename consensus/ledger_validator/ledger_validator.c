/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/ledger_validator/ledger_validator.h"
#include "common/model/milestone.h"
#include "common/storage/pack.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/tangle/tangle.h"
#include "gossip/components/requester.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

/*
 * Private functions
 */

static retcode_t update_snapshot_milestone(flex_trit_t *const hash,
                                           uint64_t index) {
  retcode_t ret = RC_OK;

  return ret;
}

static retcode_t build_snapshot(ledger_validator_t *const lv,
                                iota_milestone_t *const consistent_milestone) {
  retcode_t ret = RC_OK;
  iota_milestone_t milestone;
  iota_milestone_t *milestone_ptr = &milestone;
  iota_stor_pack_t pack = {(void **)&milestone_ptr, 1, 0, false};

  rw_lock_handle_wrlock(&lv->milestone_tracker->latest_snapshot->rw_lock);

  if ((ret = iota_tangle_milestone_load_first(lv->tangle, &pack)) != RC_OK) {
    goto done;
  }

  while (pack.num_loaded != 0) {
    pack.num_loaded = 0;
    if ((ret = iota_tangle_milestone_load_next(lv->tangle, milestone.index,
                                               &pack)) != RC_OK) {
      goto done;
    }
  }

done:
  rw_lock_handle_unlock(&lv->milestone_tracker->latest_snapshot->rw_lock);
  return RC_OK;
}

/*
 * Public functions
 */

retcode_t iota_consensus_ledger_validator_init(ledger_validator_t *const lv,
                                               tangle_t *const tangle,
                                               milestone_tracker_t *const mt,
                                               requester_state_t *const tr) {
  retcode_t ret = RC_OK;
  iota_milestone_t consistent_milestone;

  logger_helper_init(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  lv->tangle = tangle;
  lv->milestone_tracker = mt;
  mt->ledger_validator = lv;
  lv->transaction_requester = tr;

  if ((ret = build_snapshot(lv, &consistent_milestone)) != RC_OK) {
    log_critical(LEDGER_VALIDATOR_LOGGER_ID, "Building snapshot failed\n");
    return ret;
  }


retcode_t iota_consensus_ledger_validator_destroy(
    ledger_validator_t *const lv) {
  logger_helper_destroy(LEDGER_VALIDATOR_LOGGER_ID);
  lv->tangle = NULL;
  lv->milestone_tracker = NULL;
  return RC_OK;
}

retcode_t iota_consensus_ledger_validator_update_snapshot(
    ledger_validator_t *const lv, iota_milestone_t *const milestone,
    bool *const has_snapshot) {
  retcode_t ret = RC_OK;
  state_map_t diff = NULL, patch = NULL;
  struct _iota_transaction tx;
  iota_transaction_t tx_ptr = &tx;
  iota_stor_pack_t pack = {(void **)&tx_ptr, 1, 0, false};
  struct _trit_array milestone_hash = {milestone->hash, NUM_TRITS_HASH,
                                       FLEX_TRIT_SIZE_243, 0};

  rw_lock_handle_wrlock(&lv->milestone_tracker->latest_snapshot->rw_lock);

  if ((ret = iota_tangle_transaction_load(lv->tangle, TRANSACTION_COL_HASH,
                                          &milestone_hash, &pack)) != RC_OK) {
    goto done;
  } else if (pack.num_loaded != 1) {
    ret = RC_LEDGER_VALIDATOR_INVALID_MILESTONE;
    goto done;
  }
  *has_snapshot = tx.snapshot_index != 0;
  if (!(*has_snapshot)) {
    if ((ret = get_latest_diff(
             lv, &diff, tx.hash,
             iota_snapshot_get_index(lv->milestone_tracker->latest_snapshot),
             true)) != RC_OK) {
      log_error(LEDGER_VALIDATOR_LOGGER_ID, "Getting latest diff failed\n");
      goto done;
    }
    if (diff == NULL) {
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
      if ((ret = update_snapshot_milestone(milestone->hash,
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
