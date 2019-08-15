/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "ciri/consensus/tangle/tangle.h"
#include "utils/logger_helper.h"

#define TANGLE_LOGGER_ID "tangle"

static logger_id_t logger_id;

retcode_t iota_tangle_init(tangle_t *const tangle, connection_config_t const *const conf) {
  logger_id = logger_helper_enable(TANGLE_LOGGER_ID, LOGGER_DEBUG, true);
  return storage_connection_init(&tangle->connection, conf, STORAGE_CONNECTION_TANGLE);
}

retcode_t iota_tangle_destroy(tangle_t *const tangle) {
  logger_helper_release(logger_id);
  return storage_connection_destroy(&tangle->connection);
}

/*
 * Transaction operations
 */

retcode_t iota_tangle_transaction_count(tangle_t const *const tangle, size_t *const count) {
  return storage_transaction_count(&tangle->connection, count);
}

retcode_t iota_tangle_transaction_store(tangle_t const *const tangle, iota_transaction_t const *const tx) {
  return storage_transaction_store(&tangle->connection, tx);
}

retcode_t iota_tangle_transaction_load(tangle_t const *const tangle, transaction_field_t const field,
                                       flex_trit_t const *const key, iota_stor_pack_t *const tx) {
  return storage_transaction_load(&tangle->connection, field, key, tx);
}

retcode_t iota_tangle_transaction_update_solid_state(tangle_t const *const tangle, flex_trit_t const *const hash,
                                                     bool const state) {
  return storage_transaction_update_solid_state(&tangle->connection, hash, state);
}

retcode_t iota_tangle_transactions_update_solid_state(tangle_t const *const tangle, hash243_set_t const hashes,
                                                      bool const is_solid) {
  return storage_transactions_update_solid_state(&tangle->connection, hashes, is_solid);
}

retcode_t iota_tangle_transaction_load_hashes_by_address(tangle_t const *const tangle, flex_trit_t const *const address,
                                                         iota_stor_pack_t *const pack) {
  retcode_t res = RC_OK;

  res = storage_transaction_load_hashes(&tangle->connection, TRANSACTION_FIELD_ADDRESS, address, pack);

  while (res == RC_OK && pack->insufficient_capacity) {
    res = hash_pack_resize(pack, 2);
    if (res == RC_OK) {
      pack->num_loaded = 0;
      res = storage_transaction_load_hashes(&tangle->connection, TRANSACTION_FIELD_ADDRESS, address, pack);
    }
  }

  if (res != RC_OK) {
    log_error(logger_id, "Failed in loading hashes, error code is: %" PRIu64 "\n", res);
  }

  return res;
}

retcode_t iota_tangle_transaction_load_hashes_of_approvers(tangle_t const *const tangle,
                                                           flex_trit_t const *const approvee_hash,
                                                           iota_stor_pack_t *const pack, int64_t before_timestamp) {
  retcode_t res = RC_OK;

  res = storage_transaction_load_hashes_of_approvers(&tangle->connection, approvee_hash, pack, before_timestamp);

  while (res == RC_OK && pack->insufficient_capacity) {
    res = hash_pack_resize(pack, 2);
    if (res == RC_OK) {
      pack->num_loaded = 0;
      res = storage_transaction_load_hashes_of_approvers(&tangle->connection, approvee_hash, pack, before_timestamp);
    }
  }

  if (res != RC_OK) {
    log_error(logger_id, "Failed in loading approvers, error code is: %" PRIu64 "\n", res);
  }

  return res;
}

retcode_t iota_tangle_transaction_load_partial(tangle_t const *const tangle, flex_trit_t const *const hash,
                                               iota_stor_pack_t *const pack, partial_transaction_model_e models_mask) {
  if (models_mask == PARTIAL_TX_MODEL_METADATA) {
    return storage_transaction_load_metadata(&tangle->connection, hash, pack);
  } else if (models_mask == PARTIAL_TX_MODEL_ESSENCE_METADATA) {
    return storage_transaction_load_essence_and_metadata(&tangle->connection, hash, pack);
  } else if (models_mask == PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA) {
    return storage_transaction_load_essence_attachment_and_metadata(&tangle->connection, hash, pack);
  } else if (models_mask == PARTIAL_TX_MODEL_ESSENCE_CONSENSUS) {
    return storage_transaction_load_essence_and_consensus(&tangle->connection, hash, pack);
  } else {
    return RC_CONSENSUS_NOT_IMPLEMENTED;
  }
}

retcode_t iota_tangle_transaction_load_hashes_of_milestone_candidates(tangle_t const *const tangle,
                                                                      iota_stor_pack_t *const pack,
                                                                      flex_trit_t const *const coordinator) {
  retcode_t res = RC_OK;

  res = storage_transaction_load_hashes_of_milestone_candidates(&tangle->connection, pack, coordinator);

  while (res == RC_OK && pack->insufficient_capacity) {
    if ((res = hash_pack_resize(pack, 2)) == RC_OK) {
      pack->num_loaded = 0;
      res = storage_transaction_load_hashes_of_milestone_candidates(&tangle->connection, pack, coordinator);
    }
  }

  if (res != RC_OK) {
    log_error(logger_id,
              "Failed in loading hashes of milestone candidates, error code "
              "is: %" PRIu64 "\n",
              res);
  }

  return res;
}

retcode_t iota_tangle_transaction_update_snapshot_index(tangle_t const *const tangle, flex_trit_t const *const hash,
                                                        uint64_t const snapshot_index) {
  return storage_transaction_update_snapshot_index(&tangle->connection, hash, snapshot_index);
}

retcode_t iota_tangle_transactions_update_snapshot_index(tangle_t const *const tangle, hash243_set_t const hashes,
                                                         uint64_t const snapshot_index) {
  return storage_transactions_update_snapshot_index(&tangle->connection, hashes, snapshot_index);
}

retcode_t iota_tangle_transaction_exist(tangle_t const *const tangle, transaction_field_t const field,
                                        flex_trit_t const *const key, bool *const exist) {
  return storage_transaction_exist(&tangle->connection, field, key, exist);
}

retcode_t iota_tangle_transaction_approvers_count(tangle_t const *const tangle, flex_trit_t const *const hash,
                                                  size_t *const count) {
  return storage_transaction_approvers_count(&tangle->connection, hash, count);
}

retcode_t iota_tangle_transaction_find(tangle_t const *const tangle, hash243_queue_t const bundles,
                                       hash243_queue_t const addresses, hash81_queue_t const tags,
                                       hash243_queue_t const approvees, iota_stor_pack_t *const pack) {
  return storage_transaction_find(&tangle->connection, bundles, addresses, tags, approvees, pack);
}

retcode_t iota_tangle_transaction_metadata_clear(tangle_t const *const tangle) {
  return storage_transaction_metadata_clear(&tangle->connection);
}

retcode_t iota_tangle_transactions_delete(tangle_t const *const tangle, hash243_set_t const hashes) {
  return storage_transactions_delete(&tangle->connection, hashes);
}

/*
 * Bundle operations
 */

retcode_t iota_tangle_bundle_update_validity(tangle_t const *const tangle, bundle_transactions_t const *const bundle,
                                             bundle_status_t const status) {
  return storage_bundle_update_validity(&tangle->connection, bundle, status);
}

retcode_t iota_tangle_bundle_load(tangle_t const *const tangle, flex_trit_t const *const tail_hash,
                                  bundle_transactions_t *const bundle) {
  retcode_t res = RC_OK;
  uint64_t curr_index = 0;
  uint64_t last_index = 0;
  flex_trit_t *curr_trunk = NULL;
  flex_trit_t *bundle_hash = NULL;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  if ((res = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, tail_hash, &pack)) != RC_OK) {
    return res;
  }

  if (pack.num_loaded == 0) {
    return RC_TANGLE_TAIL_NOT_FOUND;
  }

  if ((curr_index = transaction_current_index(curr_tx)) != 0) {
    return RC_TANGLE_NOT_A_TAIL;
  }

  last_index = transaction_last_index(curr_tx);
  bundle_hash = transaction_bundle(curr_tx);

  while (pack.num_loaded != 0 && curr_index <= last_index &&
         memcmp(bundle_hash, transaction_bundle(curr_tx), FLEX_TRIT_SIZE_243) == 0) {
    bundle_transactions_add(bundle, curr_tx);
    curr_trunk = transaction_trunk(curr_tx);
    hash_pack_reset(&pack);
    if ((res = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, curr_trunk, &pack)) != RC_OK) {
      return res;
    }
    curr_index++;
  }

  return res;
}

/*
 * Milestone operations
 */

retcode_t iota_tangle_milestone_clear(tangle_t const *const tangle) {
  return storage_milestone_clear(&tangle->connection);
}

retcode_t iota_tangle_milestone_store(tangle_t const *const tangle, iota_milestone_t const *const data_in) {
  return storage_milestone_store(&tangle->connection, data_in);
}

retcode_t iota_tangle_milestone_load(tangle_t const *const tangle, flex_trit_t const *const hash,
                                     iota_stor_pack_t *const pack) {
  return storage_milestone_load(&tangle->connection, hash, pack);
}

retcode_t iota_tangle_milestone_load_last(tangle_t const *const tangle, iota_stor_pack_t *const pack) {
  return storage_milestone_load_last(&tangle->connection, pack);
}

retcode_t iota_tangle_milestone_load_first(tangle_t const *const tangle, iota_stor_pack_t *const pack) {
  return storage_milestone_load_first(&tangle->connection, pack);
}

retcode_t iota_tangle_milestone_load_by_index(tangle_t const *const tangle, uint64_t const index,
                                              iota_stor_pack_t *const pack) {
  return storage_milestone_load_by_index(&tangle->connection, index, pack);
}

retcode_t iota_tangle_milestone_load_next(tangle_t const *const tangle, uint64_t const index,
                                          iota_stor_pack_t *const pack) {
  return storage_milestone_load_next(&tangle->connection, index, pack);
}

retcode_t iota_tangle_milestone_exist(tangle_t const *const tangle, flex_trit_t const *const hash, bool *const exist) {
  return storage_milestone_exist(&tangle->connection, hash, exist);
}

retcode_t iota_tangle_milestone_delete(tangle_t const *const tangle, flex_trit_t const *const hash) {
  return storage_milestone_delete(&tangle->connection, hash);
}

/*
 * Utilities
 */

retcode_t iota_tangle_find_tail(tangle_t const *const tangle, flex_trit_t const *const tx_hash, flex_trit_t *const tail,
                                bool *const found_tail) {
  retcode_t res = RC_OK;
  flex_trit_t *approver_hash = NULL;
  bool found_approver = true;
  iota_stor_pack_t hash_pack;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, curr_tx_pack);
  DECLARE_PACK_SINGLE_TX(next_tx_s, next_tx, next_tx_pack);

  *found_tail = false;

  res = iota_tangle_transaction_load_partial(tangle, tx_hash, &curr_tx_pack, PARTIAL_TX_MODEL_ESSENCE_CONSENSUS);
  if (res != RC_OK || curr_tx_pack.num_loaded == 0) {
    return res;
  }

  if ((res = hash_pack_init(&hash_pack, 8)) != RC_OK) {
    return res;
  }

  for (uint32_t index = transaction_current_index(curr_tx); found_approver && index-- > 0;) {
    found_approver = false;
    approver_hash = NULL;
    hash_pack_reset(&hash_pack);

    if ((res = iota_tangle_transaction_load_hashes_of_approvers(tangle, transaction_hash(curr_tx), &hash_pack, 0)) !=
        RC_OK) {
      break;
    }

    for (uint32_t approver_index = 0; approver_index < hash_pack.num_loaded; ++approver_index) {
      approver_hash = (flex_trit_t *)hash_pack.models[approver_index];
      hash_pack_reset(&next_tx_pack);
      res = iota_tangle_transaction_load_partial(tangle, approver_hash, &next_tx_pack,
                                                 PARTIAL_TX_MODEL_ESSENCE_CONSENSUS);
      if (res != RC_OK || next_tx_pack.num_loaded == 0) {
        break;
      }
      if (transaction_current_index(next_tx) == index &&
          memcmp(transaction_bundle(next_tx), transaction_bundle(curr_tx), FLEX_TRIT_SIZE_243) == 0) {
        curr_tx = next_tx;
        found_approver = true;
        break;
      }
    }
  }

  if (transaction_current_index(curr_tx) == 0) {
    memcpy(tail, transaction_hash(curr_tx), FLEX_TRIT_SIZE_243);
    *found_tail = true;
  }

  hash_pack_free(&hash_pack);

  return res;
}

/*
 * State delta operations
 */

retcode_t iota_tangle_state_delta_store(tangle_t const *const tangle, uint64_t const index,
                                        state_delta_t const *const delta) {
  return storage_state_delta_store(&tangle->connection, index, delta);
}

retcode_t iota_tangle_state_delta_load(tangle_t const *const tangle, uint64_t const index, state_delta_t *const delta) {
  return storage_state_delta_load(&tangle->connection, index, delta);
}
