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
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "common/crypto/iss/normalize.h"
#include "common/crypto/iss/v1/iss.h"
#include "common/trinary/trit_long.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define MILESTONE_TRACKER_LOGGER_ID "milestone_tracker"
#define SOLID_MILESTONE_RESCAN_INTERVAL_MS 50ULL

static logger_id_t logger_id;

static bool is_milestone_bundle_structure_valid(bundle_transactions_t const* const bundle,
                                                iota_milestone_t const* const candidate, uint8_t const security_level) {
  iota_transaction_t* head = NULL;
  iota_transaction_t* tx = NULL;

  if ((tx = (iota_transaction_t*)utarray_eltptr(bundle, 0)) == NULL ||
      memcmp(transaction_hash(tx), candidate->hash, FLEX_TRIT_SIZE_243) != 0) {
    return false;
  }

  if ((head = (iota_transaction_t*)utarray_eltptr(bundle, security_level)) == NULL) {
    return false;
  }

  for (size_t i = 0; i < security_level; i++) {
    if ((tx = (iota_transaction_t*)utarray_eltptr(bundle, i)) == NULL) {
      return false;
    }
    if (memcmp(transaction_branch(tx), transaction_trunk(head), FLEX_TRIT_SIZE_243) != 0) {
      return false;
    }
  }

  return true;
}

// This function assumes the bundle is valid
static retcode_t validate_coordinator(milestone_tracker_t* const mt, iota_milestone_t* const candidate,
                                      bundle_transactions_t const* const bundle, bool* valid) {
  iota_transaction_t* tx = NULL;
  trit_t signature_trits[NUM_TRITS_SIGNATURE];
  trit_t siblings_trits[NUM_TRITS_SIGNATURE];
  trit_t signed_hash[HASH_LENGTH_TRIT];
  trit_t digest[mt->conf->coordinator_security_level * HASH_LENGTH_TRIT];
  trit_t root[HASH_LENGTH_TRIT];
  flex_trit_t coo[FLEX_TRIT_SIZE_243];
  sponge_t sponge;

  *valid = false;
  tx = (iota_transaction_t*)utarray_eltptr(bundle, mt->conf->coordinator_security_level);
  flex_trits_to_trits(siblings_trits, NUM_TRITS_SIGNATURE, transaction_signature(tx), NUM_TRITS_SIGNATURE,
                      NUM_TRITS_SIGNATURE);
  sponge_init(&sponge, mt->conf->coordinator_signature_type);
  normalize_flex_hash_to_trits(transaction_hash(tx), signed_hash);
  for (size_t i = 0; i < mt->conf->coordinator_security_level; i++) {
    tx = (iota_transaction_t*)utarray_eltptr(bundle, i);
    flex_trits_to_trits(signature_trits, NUM_TRITS_SIGNATURE, transaction_signature(tx), NUM_TRITS_SIGNATURE,
                        NUM_TRITS_SIGNATURE);
    iss_sig_digest(&sponge, digest + i * HASH_LENGTH_TRIT, signed_hash + i * ISS_CHUNK_LENGTH, signature_trits,
                   NUM_TRITS_SIGNATURE);
  }
  iss_address(&sponge, digest, root, mt->conf->coordinator_security_level * HASH_LENGTH_TRIT);
  iss_merkle_root(&sponge, root, siblings_trits, mt->conf->coordinator_depth, candidate->index);
  flex_trits_from_trits(coo, HASH_LENGTH_TRIT, root, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  if (memcmp(coo, mt->conf->coordinator_address, FLEX_TRIT_SIZE_243) == 0) {
    *valid = true;
  }
  sponge_destroy(&sponge);

  return RC_OK;
}

retcode_t iota_milestone_tracker_validate_milestone(milestone_tracker_t* const mt, tangle_t* const tangle,
                                                    iota_milestone_t* const candidate,
                                                    milestone_status_t* const milestone_status) {
  retcode_t ret = RC_OK;
  bundle_transactions_t* bundle = NULL;
  bool exists = false, valid = false;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  *milestone_status = MILESTONE_INVALID;

  if (candidate->index >= mt->conf->coordinator_max_milestone_index) {
    *milestone_status = MILESTONE_INVALID;
    return ret;
  } else if ((candidate->index <= mt->latest_solid_milestone_index && mt->latest_solid_milestone_index != 0) ||
             (candidate->index == mt->latest_milestone_index && mt->latest_milestone_index != 0)) {
    *milestone_status = MILESTONE_EXISTS;
    return ret;
  }

  // Check if milestone is already present in database i.e. validated
  if ((ret = iota_tangle_milestone_exist(tangle, candidate->hash, &exists)) != RC_OK) {
    return ret;
  } else if (exists) {
    *milestone_status = MILESTONE_EXISTS;
    return ret;
  }

  bundle_transactions_new(&bundle);
  if (bundle == NULL) {
    return RC_OOM;
  }
  if ((ret = iota_consensus_bundle_validator_validate(tangle, candidate->hash, bundle, &bundle_status)) != RC_OK) {
    log_warning(logger_id, "Validating bundle failed\n");
    goto done;
  } else if (bundle_status == BUNDLE_INCOMPLETE) {
    *milestone_status = MILESTONE_INCOMPLETE;
    goto done;
  } else if (bundle_status == BUNDLE_VALID) {
    if (!is_milestone_bundle_structure_valid(bundle, candidate, mt->conf->coordinator_security_level)) {
      log_warning(logger_id, "Invalid milestone bundle structure\n");
      goto done;
    }

    if ((ret = validate_coordinator(mt, candidate, bundle, &valid)) != RC_OK) {
      log_warning(logger_id, "Validating coordinator failed\n");
      goto done;
    }

    if (valid) {
      *milestone_status = MILESTONE_VALID;
    }
  } else {
    log_debug(logger_id, "Abnormal bundle_status %d\n", bundle_status);
  }

done:
  if (bundle) {
    bundle_transactions_free(&bundle);
  }
  return ret;
}

uint64_t iota_milestone_tracker_get_milestone_index(iota_transaction_t* const tx) {
  trit_t buffer[NUM_TRITS_OBSOLETE_TAG];

  flex_trits_to_trits(buffer, NUM_TRITS_OBSOLETE_TAG, transaction_obsolete_tag(tx), NUM_TRITS_OBSOLETE_TAG,
                      NUM_TRITS_OBSOLETE_TAG);
  return trits_to_long(buffer, NUM_TRITS_VALUE);
}

static void* milestone_validator(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  iota_milestone_t candidate;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  hash243_queue_entry_t* entry = NULL;
  milestone_status_t milestone_status;
  lock_handle_t lock_cond;
  tangle_t tangle;
  bool is_solid;

  if (mt == NULL) {
    return NULL;
  }

  {
    storage_connection_config_t db_conf = {.db_path = mt->conf->tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (mt->running) {
    lock_handle_lock(&mt->candidates_lock);
    entry = hash243_queue_pop(&mt->candidates);
    lock_handle_unlock(&mt->candidates_lock);

    if (entry == NULL) {
      cond_handle_wait(&mt->cond_validator, &lock_cond);
      continue;
    }

    memcpy(candidate.hash, entry->hash, FLEX_TRIT_SIZE_243);
    free(entry);
    hash_pack_reset(&pack);
    if (iota_tangle_transaction_load_partial(&tangle, candidate.hash, &pack, PARTIAL_TX_MODEL_ESSENCE_CONSENSUS) ==
            RC_OK &&
        pack.num_loaded != 0) {
      candidate.index = iota_milestone_tracker_get_milestone_index(&tx);
      if (iota_milestone_tracker_validate_milestone(mt, &tangle, &candidate, &milestone_status) != RC_OK) {
        log_warning(logger_id, "Validating milestone failed\n");
        continue;
      }
      if (milestone_status == MILESTONE_VALID) {
        iota_tangle_milestone_store(&tangle, &candidate);
        if (candidate.index > mt->latest_milestone_index) {
          log_info(logger_id,
                   "Latest milestone was changed from #%" PRIu64 " to #%" PRIu64 " (%d remaining candidates)\n",
                   mt->latest_milestone_index, candidate.index, hash243_queue_count(mt->candidates));
          mt->latest_milestone_index = candidate.index;
          memcpy(mt->latest_milestone, candidate.hash, FLEX_TRIT_SIZE_243);
        }
      } else if (milestone_status == MILESTONE_INCOMPLETE) {
        if (iota_consensus_transaction_solidifier_check_solidity(mt->transaction_solidifier, &tangle, candidate.hash,
                                                                 MILESTONE_VALIDATION_TRANSACTIONS_LIMIT,
                                                                 &is_solid) != RC_OK) {
          log_warning(logger_id, "Quick fetching of milestone failed\n");
        }
        iota_milestone_tracker_add_candidate(mt, candidate.hash);
      }
    }
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  return NULL;
}

retcode_t update_latest_solid_milestone(milestone_tracker_t* const mt, tangle_t* const tangle) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);
  bool has_snapshot = false;
  bool is_solid = false;

  if (mt == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = iota_tangle_milestone_load_next(tangle, mt->latest_solid_milestone_index, &pack)) != RC_OK) {
    return ret;
  }

  while (pack.num_loaded != 0 && milestone.index <= mt->latest_milestone_index && mt->running) {
    has_snapshot = false;
    is_solid = false;

    if ((ret = iota_consensus_transaction_solidifier_check_solidity(mt->transaction_solidifier, tangle, milestone.hash,
                                                                    MILESTONE_SOLIDIFICATION_TRANSACTIONS_LIMIT,
                                                                    &is_solid)) != RC_OK) {
      return ret;
    }
    if (!is_solid) {
      break;
    }

    if (milestone.index == mt->latest_solid_milestone_index + 1) {
      if ((ret = iota_consensus_ledger_validator_update_snapshot(mt->ledger_validator, tangle, &milestone,
                                                                 &has_snapshot)) != RC_OK) {
        log_error(logger_id, "Updating snapshot failed\n");
        return ret;
      } else if (has_snapshot) {
        mt->latest_solid_milestone_index = milestone.index;
        memcpy(mt->latest_solid_milestone, milestone.hash, FLEX_TRIT_SIZE_243);
      } else {
        break;
      }
    }
    pack.num_loaded = 0;
    if ((ret = iota_tangle_milestone_load_next(tangle, mt->latest_solid_milestone_index, &pack)) != RC_OK) {
      return ret;
    }
  }
  return ret;
}

static void* milestone_solidifier(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  uint64_t previous_solid_latest_milestone_index = 0;
  lock_handle_t lock_cond;
  tangle_t tangle;

  if (mt == NULL) {
    return NULL;
  }

  {
    storage_connection_config_t db_conf = {.db_path = mt->conf->tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (mt->running) {
    log_debug(logger_id, "Scanning for latest solid milestone\n");
    if (mt->latest_solid_milestone_index < mt->latest_milestone_index) {
      previous_solid_latest_milestone_index = mt->latest_solid_milestone_index;
      if (update_latest_solid_milestone(mt, &tangle) != RC_OK) {
        log_warning(logger_id, "Updating latest solid milestone failed\n");
      }
      if (previous_solid_latest_milestone_index != mt->latest_solid_milestone_index) {
        log_info(logger_id,
                 "Latest solid milestone was changed from #%" PRIu64 " to #%" PRIu64 " (%d remaining candidates)\n",
                 previous_solid_latest_milestone_index, mt->latest_solid_milestone_index,
                 mt->latest_milestone_index - mt->latest_solid_milestone_index);
        continue;
      }
    }
    cond_handle_timedwait(&mt->cond_solidifier, &lock_cond, SOLID_MILESTONE_RESCAN_INTERVAL_MS);
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  return NULL;
}

retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt, iota_consensus_conf_t* const conf,
                                      snapshots_provider_t* const snapshots_provider, ledger_validator_t* const lv,
                                      transaction_solidifier_t* const ts) {
  if (mt == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(MILESTONE_TRACKER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(mt, 0, sizeof(milestone_tracker_t));
  mt->running = false;
  mt->conf = conf;
  mt->ledger_validator = lv;
  mt->transaction_solidifier = ts;
  mt->candidates = NULL;
  lock_handle_init(&mt->candidates_lock);
  mt->milestone_start_index = conf->last_milestone;
  mt->latest_milestone_index = conf->last_milestone;
  mt->latest_solid_milestone_index = MAX(conf->last_milestone, snapshots_provider->initial_snapshot.metadata.index);
  mt->snapshots_provider = snapshots_provider;
  cond_handle_init(&mt->cond_validator);
  cond_handle_init(&mt->cond_solidifier);

  return RC_OK;
}

retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt, tangle_t* const tangle) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_MILESTONE(latest_milestone, latest_milestone_ptr, pack);
  iota_stor_pack_t hash_pack;
  flex_trit_t* curr_hash;
  hash243_set_t solid_entry_points = NULL;

  if (mt == NULL) {
    return RC_NULL_PARAM;
  }

  iota_snapshot_solid_entry_points_set(&mt->snapshots_provider->initial_snapshot, &solid_entry_points);

  mt->running = true;

  if ((ret = iota_tangle_milestone_load_last(tangle, &pack)) != RC_OK) {
    return ret;
  }
  if (pack.num_loaded != 0) {
    mt->latest_milestone_index = MAX(latest_milestone.index, mt->snapshots_provider->latest_snapshot.metadata.index);
    memcpy(mt->latest_milestone, latest_milestone.hash, FLEX_TRIT_SIZE_243);
  }
  log_info(logger_id, "Latest milestone: #%d\n", mt->latest_milestone_index);

  hash_pack_init(&hash_pack, 512);

  if ((ret = iota_tangle_transaction_load_hashes_of_milestone_candidates(tangle, mt->conf->coordinator_address,
                                                                         &hash_pack)) != RC_OK) {
    log_critical(logger_id, "Loading milestone candidates failed\n");
  }
  log_info(logger_id, "Loaded %d milestone candidates\n", hash_pack.num_loaded);

  for (size_t i = 0; i < hash_pack.num_loaded; i++) {
    curr_hash = ((flex_trit_t**)hash_pack.models)[i];
    if (!hash243_set_contains(solid_entry_points, curr_hash)) {
      iota_milestone_tracker_add_candidate(mt, curr_hash);
    }
  }
  hash_pack_free(&hash_pack);
  hash243_set_free(&solid_entry_points);

  log_info(logger_id, "Spawning milestone validator thread\n");
  if (thread_handle_create(&mt->milestone_validator, (thread_routine_t)milestone_validator, mt) != 0) {
    log_critical(logger_id, "Spawning milestone validator thread failed\n");
    return RC_THREAD_CREATE;
  }

  log_info(logger_id, "Latest solid milestone: #%d\n", mt->latest_solid_milestone_index);

  log_info(logger_id, "Spawning milestone solidifier thread\n");
  if (thread_handle_create(&mt->milestone_solidifier, (thread_routine_t)milestone_solidifier, mt) != 0) {
    log_critical(logger_id, "Spawning milestone solidifier thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_NULL_PARAM;
  } else if (mt->running == false) {
    return RC_OK;
  }

  mt->running = false;

  log_info(logger_id, "Shutting down milestone validator thread\n");
  cond_handle_signal(&mt->cond_validator);
  if (thread_handle_join(mt->milestone_validator, NULL) != 0) {
    log_error(logger_id, "Shutting down milestone validator thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  log_info(logger_id, "Shutting down milestone solidifier thread\n");
  cond_handle_signal(&mt->cond_solidifier);
  if (thread_handle_join(mt->milestone_solidifier, NULL) != 0) {
    log_error(logger_id, "Shutting down milestone solidifier thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_NULL_PARAM;
  } else if (mt->running) {
    return RC_STILL_RUNNING;
  }

  hash243_queue_free(&mt->candidates);
  lock_handle_destroy(&mt->candidates_lock);
  cond_handle_destroy(&mt->cond_validator);
  cond_handle_destroy(&mt->cond_solidifier);
  memset(mt, 0, sizeof(milestone_tracker_t));
  logger_helper_release(logger_id);

  return ret;
}

retcode_t iota_milestone_tracker_add_candidate(milestone_tracker_t* const mt, flex_trit_t const* const hash) {
  retcode_t ret = RC_OK;

  if (mt == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  lock_handle_lock(&mt->candidates_lock);
  ret = hash243_queue_push(&mt->candidates, hash);
  lock_handle_unlock(&mt->candidates_lock);

  if (ret != RC_OK) {
    log_warning(logger_id, "Pushing candidate hash to candidates queue failed\n");
    return RC_OOM;
  }
  cond_handle_signal(&mt->cond_validator);

  return RC_OK;
}
