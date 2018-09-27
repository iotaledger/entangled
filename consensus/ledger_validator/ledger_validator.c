/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utlist.h"

#include "common/model/milestone.h"
#include "common/storage/pack.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/tangle/tangle.h"
#include "gossip/components/requester.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

// TODO move ?
typedef struct hash_queue_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  struct hash_queue_s *next;
  struct hash_queue_s *prev;
} hash_queue_t;

// TODO move ?
typedef struct hash_set_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  UT_hash_handle hh;
} hash_set_t;

/*
 * Private functions
 */

static retcode_t update_snapshot_milestone(ledger_validator_t *const lv,
                                           flex_trit_t *const milestone_hash,
                                           uint64_t index) {
  retcode_t ret = RC_OK;
  hash_queue_t *non_analyzed_hashes = NULL, *hash_queue_elem = NULL,
               *tmp = NULL;
  hash_set_t *analyzed_hashes = NULL, *hash_set_elem = NULL;

  struct _iota_transaction tx;
  iota_transaction_t tx_ptr = &tx;
  iota_stor_pack_t pack = {(void **)&tx_ptr, 1, 0, false};
  struct _trit_array tx_hash = {NULL, NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  // Add milestone hash to the queue as entry point for the traversal
  if ((hash_queue_elem = malloc(sizeof(hash_queue_t))) == NULL) {
    ret = RC_LEDGER_VALIDATOR_OOM;
    goto done;
  }
  memcpy(hash_queue_elem->hash, milestone_hash, FLEX_TRIT_SIZE_243);
  CDL_APPEND(non_analyzed_hashes, hash_queue_elem);

  while (non_analyzed_hashes != NULL) {
    tx_hash.trits = non_analyzed_hashes->hash;
    HASH_FIND(hh, analyzed_hashes, tx_hash.trits, FLEX_TRIT_SIZE_243,
              hash_set_elem);
    if (hash_set_elem == NULL) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load(lv->tangle, TRANSACTION_COL_HASH,
                                              &tx_hash, &pack)) != RC_OK) {
        goto done;
      }
      if (tx.snapshot_index == 0) {
        // TODO update snapshot index
        // TODO messageQ publish
        // Add trunk hash to the queue
        if ((hash_queue_elem = malloc(sizeof(hash_queue_t))) == NULL) {
          ret = RC_LEDGER_VALIDATOR_OOM;
          goto done;
        }
        memcpy(hash_queue_elem->hash, tx.trunk, FLEX_TRIT_SIZE_243);
        CDL_APPEND(non_analyzed_hashes, hash_queue_elem);
        // Add branch hash to the queue
        if ((hash_queue_elem = malloc(sizeof(hash_queue_t))) == NULL) {
          ret = RC_LEDGER_VALIDATOR_OOM;
          goto done;
        }
        memcpy(hash_queue_elem->hash, tx.branch, FLEX_TRIT_SIZE_243);
        CDL_APPEND(non_analyzed_hashes, hash_queue_elem);
      }
      // Mark current hash as visited
      if ((hash_set_elem = malloc(sizeof(hash_set_t))) == NULL) {
        ret = RC_LEDGER_VALIDATOR_OOM;
        goto done;
      }
      memcpy(hash_set_elem->hash, tx_hash.trits, FLEX_TRIT_SIZE_243);
      HASH_ADD(hh, analyzed_hashes, hash, FLEX_TRIT_SIZE_243, hash_set_elem);
    }
    // Remove current hash from the queue
    tmp = non_analyzed_hashes;
    CDL_DELETE(non_analyzed_hashes, non_analyzed_hashes);
    free(tmp);
  }

done : {
  hash_queue_t *iter_q, *tmp_q1, *tmp_q2;
  hash_set_t *iter_s, *tmp_s;

  CDL_FOREACH_SAFE(non_analyzed_hashes, iter_q, tmp_q1, tmp_q2) {
    CDL_DELETE(non_analyzed_hashes, iter_q);
    free(iter_q);
  }

  HASH_ITER(hh, analyzed_hashes, iter_s, tmp_s) {
    HASH_DEL(analyzed_hashes, iter_s);
    free(iter_s);
  }
}
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
