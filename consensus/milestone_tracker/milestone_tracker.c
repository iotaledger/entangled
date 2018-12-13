/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdlib.h>

#include "common/model/milestone.h"
#include "common/sign/normalize.h"
#include "common/sign/v1/iss_curl.h"
#include "common/storage/sql/defs.h"
#include "common/trinary/trit_array.h"
#include "common/trinary/trit_long.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/transaction_solidifier/transaction_solidifier.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/merkle.h"
#include "utils/time.h"

#define MILESTONE_TRACKER_LOGGER_ID "milestone_tracker"

static retcode_t validate_coordinator(milestone_tracker_t* const mt,
                                      iota_milestone_t* const candidate,
                                      iota_transaction_t tx1,
                                      iota_transaction_t tx2, bool* valid) {
  trit_t signature_trits[NUM_TRITS_SIGNATURE];
  trit_t siblings_trits[NUM_TRITS_SIGNATURE];
  trit_t normalized_trunk_trits[HASH_LENGTH_TRIT];
  trit_t sig_digest[HASH_LENGTH_TRIT];
  trit_t root[HASH_LENGTH_TRIT];
  flex_trit_t coo[FLEX_TRIT_SIZE_243];
  Curl curl;

  *valid = false;

  flex_trits_to_trits(signature_trits, NUM_TRITS_SIGNATURE,
                      transaction_signature(tx1), NUM_TRITS_SIGNATURE,
                      NUM_TRITS_SIGNATURE);
  flex_trits_to_trits(siblings_trits, NUM_TRITS_SIGNATURE,
                      transaction_signature(tx2), NUM_TRITS_SIGNATURE,
                      NUM_TRITS_SIGNATURE);
  curl.type = CURL_P_27;
  init_curl(&curl);
  normalize_hash_trits(transaction_trunk(tx1), normalized_trunk_trits);
  iss_curl_sig_digest(sig_digest, normalized_trunk_trits, signature_trits,
                      NUM_TRITS_SIGNATURE, &curl);
  curl_reset(&curl);
  iss_curl_address(sig_digest, root, HASH_LENGTH_TRIT, &curl);
  merkle_root(root, siblings_trits, mt->conf->num_keys_in_milestone,
              candidate->index, &curl);
  flex_trits_from_trits(coo, HASH_LENGTH_TRIT, root, HASH_LENGTH_TRIT,
                        HASH_LENGTH_TRIT);

  if (memcmp(coo, mt->coordinator->trits, FLEX_TRIT_SIZE_243) == 0) {
    *valid = true;
  }
  return RC_OK;
}

typedef enum milestone_status_e {
  MILESTONE_VALID,
  MILESTONE_INVALID,
  MILESTONE_EXISTS,
  MILESTONE_INCOMPLETE,
} milestone_status_t;

static retcode_t validate_milestone(
    milestone_tracker_t* const mt, iota_milestone_t* const candidate,
    milestone_status_t* const milestone_status) {
  retcode_t ret = RC_OK;
  bundle_transactions_t* bundle = NULL;
  bool exists = false, valid = false;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  *milestone_status = MILESTONE_INVALID;

  if (candidate->index >= 0x200000) {
    *milestone_status = MILESTONE_INVALID;
    return ret;
  } else if (candidate->index <= mt->latest_solid_subtangle_milestone_index ||
             candidate->index == mt->latest_milestone_index) {
    *milestone_status = MILESTONE_EXISTS;
    return ret;
  }

  // Check if milestone is already present in database i.e. validated
  TRIT_ARRAY_DECLARE(hash, HASH_LENGTH_TRIT);
  memcpy(hash.trits, candidate->hash, FLEX_TRIT_SIZE_243);
  if ((ret = iota_tangle_milestone_exist(mt->tangle, candidate->hash,
                                         &exists)) != RC_OK) {
    return ret;
  } else if (exists) {
    *milestone_status = MILESTONE_EXISTS;
    return ret;
  }

  bundle_transactions_new(&bundle);
  if (bundle == NULL) {
    return RC_CONSENSUS_MT_OOM;
  }
  if ((ret = iota_consensus_bundle_validator_validate(
           mt->tangle, &hash, bundle, &bundle_status)) != RC_OK) {
    log_warning(MILESTONE_TRACKER_LOGGER_ID, "Validating bundle failed\n");
    goto done;
  } else if (bundle_status == BUNDLE_INCOMPLETE) {
    *milestone_status = MILESTONE_INCOMPLETE;
    goto done;
  } else if (bundle_status == BUNDLE_VALID) {
    iota_transaction_t tx1 = NULL;
    iota_transaction_t tx2 = NULL;

    if ((tx1 = (iota_transaction_t)utarray_eltptr(bundle, 0)) == NULL ||
        memcmp(transaction_hash(tx1), candidate->hash, FLEX_TRIT_SIZE_243) !=
            0) {
      goto done;
    }
    if ((tx2 = (iota_transaction_t)utarray_eltptr(bundle, 1)) == NULL ||
        memcmp(transaction_branch(tx1), transaction_trunk(tx2),
               FLEX_TRIT_SIZE_243) != 0) {
      goto done;
    }

    if ((ret = validate_coordinator(mt, candidate, tx1, tx2, &valid)) !=
        RC_OK) {
      log_warning(MILESTONE_TRACKER_LOGGER_ID,
                  "Validating coordinator failed\n");
      goto done;
    }
    if (valid) {
      *milestone_status = MILESTONE_VALID;
    }
  }

done:
  if (bundle) {
    bundle_transactions_free(&bundle);
  }
  return ret;
}

static uint64_t get_milestone_index(iota_transaction_t const tx) {
  trit_t buffer[NUM_TRITS_OBSOLETE_TAG];

  flex_trits_to_trits(buffer, NUM_TRITS_OBSOLETE_TAG,
                      transaction_obsolete_tag(tx), NUM_TRITS_OBSOLETE_TAG,
                      NUM_TRITS_OBSOLETE_TAG);
  return trits_to_long(buffer, NUM_TRITS_VALUE);
}

static void* milestone_validator(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  iota_milestone_t candidate;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  flex_trit_t* peek = NULL;
  milestone_status_t milestone_status;

  if (mt == NULL) {
    return NULL;
  }

  while (mt->running) {
    rw_lock_handle_wrlock(&mt->candidates_lock);
    peek = hash243_queue_peek(mt->candidates);

    if (peek != NULL) {
      memcpy(candidate.hash, peek, FLEX_TRIT_SIZE_243);
      hash243_queue_pop(&mt->candidates);
      rw_lock_handle_unlock(&mt->candidates_lock);
      hash_pack_reset(&pack);
      if (iota_tangle_transaction_load_partial(
              mt->tangle, candidate.hash, &pack,
              PARTIAL_TX_MODEL_ESSENCE_CONSENSUS) == RC_OK &&
          pack.num_loaded != 0) {
        candidate.index = get_milestone_index(&tx);
        if (validate_milestone(mt, &candidate, &milestone_status) != RC_OK) {
          log_warning(MILESTONE_TRACKER_LOGGER_ID,
                      "Validating milestone failed\n");
          continue;
        }
        if (milestone_status == MILESTONE_VALID) {
          iota_tangle_milestone_store(mt->tangle, &candidate);
          if (candidate.index > mt->latest_milestone_index) {
            log_info(MILESTONE_TRACKER_LOGGER_ID,
                     "Latest milestone has changed from #%" PRIu64
                     " to #%" PRIu64 " (%d remaining candidates)\n",
                     mt->latest_milestone_index, candidate.index,
                     hash243_queue_count(mt->candidates));
            mt->latest_milestone_index = candidate.index;
            memcpy(mt->latest_milestone->trits, candidate.hash,
                   FLEX_TRIT_SIZE_243);
          }
        } else if (milestone_status == MILESTONE_INCOMPLETE) {
          iota_milestone_tracker_add_candidate(mt, candidate.hash);
        }
      }
    } else {
      rw_lock_handle_unlock(&mt->candidates_lock);
    }
    sleep_ms(MILESTONE_VALIDATION_INTERVAL);
  }

  return NULL;
}

static retcode_t update_latest_solid_subtangle_milestone(
    milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);
  bool has_snapshot = false;
  bool is_solid = false;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  if ((ret = iota_tangle_milestone_load_next(
           mt->tangle, mt->latest_solid_subtangle_milestone_index, &pack)) !=
      RC_OK) {
    return ret;
  }

  while (pack.num_loaded != 0 &&
         milestone.index <= mt->latest_milestone_index && mt->running) {
    has_snapshot = false;
    is_solid = false;
    if (milestone.index > mt->latest_solid_subtangle_milestone_index) {
      if ((ret = iota_consensus_transaction_solidifier_check_solidity(
               mt->transaction_solidifier, milestone.hash, true, &is_solid)) !=
          RC_OK) {
        return ret;
      }
      if (!is_solid) {
        break;
      }
      if ((ret = iota_consensus_ledger_validator_update_snapshot(
               mt->ledger_validator, &milestone, &has_snapshot)) != RC_OK) {
        log_error(MILESTONE_TRACKER_LOGGER_ID, "Updating snapshot failed\n");
        return ret;
      } else if (has_snapshot) {
        mt->latest_solid_subtangle_milestone_index = milestone.index;
        memcpy(mt->latest_solid_subtangle_milestone->trits, milestone.hash,
               FLEX_TRIT_SIZE_243);
      } else {
        break;
      }
    } else {
      break;
    }
    pack.num_loaded = 0;
    if ((ret = iota_tangle_milestone_load_next(
             mt->tangle, mt->latest_solid_subtangle_milestone_index, &pack)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

static void* milestone_solidifier(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  uint64_t scan_time = 0;
  uint64_t previous_solid_subtangle_latest_milestone_index = 0;

  if (mt != NULL) {
    while (mt->running) {
      log_debug(MILESTONE_TRACKER_LOGGER_ID,
                "Scanning for latest solid subtangle milestone\n");
      scan_time = current_timestamp_ms();
      previous_solid_subtangle_latest_milestone_index =
          mt->latest_solid_subtangle_milestone_index;
      if (mt->latest_solid_subtangle_milestone_index <
          mt->latest_milestone_index) {
        if (update_latest_solid_subtangle_milestone(mt) != RC_OK) {
          log_warning(MILESTONE_TRACKER_LOGGER_ID,
                      "Updating latest solid subtangle milestone failed\n");
        }
      }
      if (previous_solid_subtangle_latest_milestone_index !=
          mt->latest_solid_subtangle_milestone_index) {
        // TODO messageQ publish lmsi/lmhs
        log_info(MILESTONE_TRACKER_LOGGER_ID,
                 "Latest solid subtangle milestone has changed from #%" PRIu64
                 " to #%" PRIu64 "\n",
                 previous_solid_subtangle_latest_milestone_index,
                 mt->latest_solid_subtangle_milestone_index);
      }
      sleep_ms(MAX(1, SOLID_MILESTONE_RESCAN_INTERVAL -
                          (current_timestamp_ms() - scan_time)));
    }
  }
  return NULL;
}

retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt,
                                      iota_consensus_conf_t* const conf,
                                      tangle_t* const tangle,
                                      snapshot_t* const snapshot,
                                      ledger_validator_t* const lv,
                                      transaction_solidifier_t* const ts) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (tangle == NULL) {
    return RC_CONSENSUS_MT_NULL_TANGLE;
  }

  logger_helper_init(MILESTONE_TRACKER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(mt, 0, sizeof(milestone_tracker_t));
  mt->running = false;
  mt->conf = conf;
  mt->tangle = tangle;
  mt->latest_snapshot = snapshot;
  mt->ledger_validator = lv;
  mt->transaction_solidifier = ts;
  mt->candidates = NULL;
  rw_lock_handle_init(&mt->candidates_lock);
  if ((mt->latest_milestone = trit_array_new(NUM_TRITS_HASH)) == NULL) {
    goto oom;
  }
  if ((mt->latest_solid_subtangle_milestone =
           trit_array_new(HASH_LENGTH_TRIT)) == NULL) {
    goto oom;
  }
  if ((mt->coordinator = trit_array_new(HASH_LENGTH_TRIT)) == NULL) {
    goto oom;
  }
  memcpy(mt->coordinator->trits, conf->coordinator, FLEX_TRIT_SIZE_243);
  mt->milestone_start_index = conf->last_milestone;
  mt->latest_milestone_index = conf->last_milestone;
  mt->latest_solid_subtangle_milestone_index = conf->last_milestone;

  return RC_OK;

oom:
  if (mt->latest_milestone) {
    trit_array_free(mt->latest_milestone);
  }
  if (mt->latest_solid_subtangle_milestone) {
    trit_array_free(mt->latest_solid_subtangle_milestone);
  }
  if (mt->coordinator) {
    trit_array_free(mt->coordinator);
  }
  return RC_CONSENSUS_MT_OOM;
}

retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_MILESTONE(latest_milestone, latest_milestone_ptr, pack);
  iota_stor_pack_t hash_pack;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  mt->running = true;

  if ((ret = iota_tangle_milestone_load_last(mt->tangle, &pack)) != RC_OK) {
    return ret;
  }
  if (pack.num_loaded != 0) {
    mt->latest_milestone_index = latest_milestone.index;
    memcpy(mt->latest_milestone->trits, latest_milestone.hash,
           FLEX_TRIT_SIZE_243);
  }
  log_info(MILESTONE_TRACKER_LOGGER_ID, "Latest milestone: #%d\n",
           mt->latest_milestone_index);

  hash_pack_init(&hash_pack, 512);

  if ((ret = iota_tangle_transaction_load_hashes_of_milestone_candidates(
           mt->tangle, &hash_pack, mt->coordinator->trits)) != RC_OK) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Loading milestone candidates failed\n");
  }
  log_info(MILESTONE_TRACKER_LOGGER_ID, "Loaded %d milestone candidates\n",
           hash_pack.num_loaded);

  for (size_t i = 0; i < hash_pack.num_loaded; i++) {
    iota_milestone_tracker_add_candidate(
        mt, ((trit_array_p*)hash_pack.models)[i]->trits);
  }
  hash_pack_free(&hash_pack);

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Spawning milestone validator thread\n");
  if (thread_handle_create(&mt->milestone_validator,
                           (thread_routine_t)milestone_validator, mt) != 0) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Spawning milestone validator thread failed\n");
    return RC_CONSENSUS_MT_FAILED_THREAD_SPAWN;
  }

  log_info(MILESTONE_TRACKER_LOGGER_ID, "Latest solid milestone: #%d\n",
           mt->latest_solid_subtangle_milestone_index);

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Spawning milestone solidifier thread\n");
  if (thread_handle_create(&mt->milestone_solidifier,
                           (thread_routine_t)milestone_solidifier, mt) != 0) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Spawning milestone solidifier thread failed\n");
    return RC_CONSENSUS_MT_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (mt->running == false) {
    return RC_OK;
  }

  mt->running = false;

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Shutting down milestone validator thread\n");
  if (thread_handle_join(mt->milestone_validator, NULL) != 0) {
    log_error(MILESTONE_TRACKER_LOGGER_ID,
              "Shutting down milestone validator thread failed\n");
    ret = RC_CONSENSUS_MT_FAILED_THREAD_JOIN;
  }

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Shutting down milestone solidifier thread\n");
  if (thread_handle_join(mt->milestone_solidifier, NULL) != 0) {
    log_error(MILESTONE_TRACKER_LOGGER_ID,
              "Shutting down milestone solidifier thread failed\n");
    ret = RC_CONSENSUS_MT_FAILED_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (mt->running) {
    return RC_CONSENSUS_MT_STILL_RUNNING;
  }

  trit_array_free(mt->latest_milestone);
  trit_array_free(mt->latest_solid_subtangle_milestone);
  trit_array_free(mt->coordinator);
  hash243_queue_free(&mt->candidates);
  rw_lock_handle_destroy(&mt->candidates_lock);
  memset(mt, 0, sizeof(milestone_tracker_t));
  logger_helper_destroy(MILESTONE_TRACKER_LOGGER_ID);

  return ret;
}

retcode_t iota_milestone_tracker_add_candidate(milestone_tracker_t* const mt,
                                               flex_trit_t const* const hash) {
  retcode_t ret = RC_OK;

  if (mt == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&mt->candidates_lock);
  ret = hash243_queue_push(&mt->candidates, hash);
  rw_lock_handle_unlock(&mt->candidates_lock);

  if (ret != RC_OK) {
    log_warning(MILESTONE_TRACKER_LOGGER_ID,
                "Pushing candidate hash to candidates queue failed\n");
    return RC_OOM;
  }

  return RC_OK;
}
