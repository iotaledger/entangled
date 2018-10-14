/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "consensus/tangle/tangle.h"
#include "utils/logger_helper.h"

#define TANGLE_LOGGER_ID "consensus_tangle"

retcode_t iota_tangle_init(tangle_t *tangle, const connection_config_t *conf) {
  logger_helper_init(TANGLE_LOGGER_ID, LOGGER_DEBUG, true);
  return iota_stor_init(&tangle->conn, conf);
}
retcode_t iota_tangle_destroy(tangle_t *tangle) {
  logger_helper_destroy(TANGLE_LOGGER_ID);
  return iota_stor_destroy(&tangle->conn);
}

/*
 * Transaction operations
 */

retcode_t iota_tangle_transaction_store(const tangle_t *const tangle,
                                        const iota_transaction_t tx) {
  return iota_stor_transaction_store(&tangle->conn, tx);
}

retcode_t iota_tangle_transaction_load(const tangle_t *const tangle,
                                       transaction_field_t const field,
                                       const trit_array_p key,
                                       iota_stor_pack_t *tx) {
  // TODO - when more "persistance provider are implemented - should loop
  return iota_stor_transaction_load(&tangle->conn, field, key, tx);
}

retcode_t iota_tangle_transaction_load_hashes(const tangle_t *const tangle,
                                              transaction_field_t const field,
                                              const trit_array_p key,
                                              iota_stor_pack_t *pack) {
  retcode_t res = RC_OK;
  fprintf(stderr, "ENTER\n");

  res = iota_stor_transaction_load_hashes(&tangle->conn, field, key, pack);
  fprintf(stderr, "%d\n", res);
  while (res == RC_OK && pack->insufficient_capacity) {
    res = hash_pack_resize(pack, 2);
    if (res == RC_OK) {
      pack->num_loaded = 0;
      res = iota_stor_transaction_load_hashes(&tangle->conn, field, key, pack);
    }
  }

  if (res != RC_OK) {
    log_error(TANGLE_LOGGER_ID,
              "Failed in loading hashes, error code is: %" PRIu64 "\n", res);
  }
  fprintf(stderr, "LEAVE\n");
  return res;
}

retcode_t iota_tangle_transaction_load_hashes_of_approvers(
    const tangle_t *const tangle, const flex_trit_t *approvee_hash,
    iota_stor_pack_t *pack) {
  retcode_t res = RC_OK;

  res = iota_stor_transaction_load_hashes_of_approvers(&tangle->conn,
                                                       approvee_hash, pack);

  while (res == RC_OK && pack->insufficient_capacity) {
    res = hash_pack_resize(pack, 2);
    if (res == RC_OK) {
      pack->num_loaded = 0;
      res = iota_stor_transaction_load_hashes_of_approvers(&tangle->conn,
                                                           approvee_hash, pack);
    }
  }

  if (res != RC_OK) {
    log_error(TANGLE_LOGGER_ID,
              "Failed in loading approvers, error code is: %" PRIu64 "\n", res);
  }

  return res;
}

retcode_t iota_tangle_transaction_update_snapshot_index(
    const tangle_t *const tangle, flex_trit_t *const hash,
    uint64_t snapshot_index) {
  return iota_stor_transaction_update_snapshot_index(&tangle->conn, hash,
                                                     snapshot_index);
}

retcode_t iota_tangle_transaction_exist(const tangle_t *const tangle,
                                        transaction_field_t const field,
                                        const trit_array_p key, bool *exist) {
  // TODO - when more "persistance provider are implemented - should loop
  return iota_stor_transaction_exist(&tangle->conn, field, key, exist);
}

/*
 * Milestone operations
 */

retcode_t iota_tangle_milestone_store(const tangle_t *const tangle,
                                      const iota_milestone_t *data_in) {
  return iota_stor_milestone_store(&tangle->conn, data_in);
}

retcode_t iota_tangle_milestone_load(const tangle_t *const tangle,
                                     flex_trit_t const *const hash,
                                     iota_stor_pack_t *pack) {
  return iota_stor_milestone_load(&tangle->conn, hash, pack);
}

retcode_t iota_tangle_milestone_load_first(const tangle_t *const tangle,
                                           iota_stor_pack_t *pack) {
  return iota_stor_milestone_load_first(&tangle->conn, pack);
}

retcode_t iota_tangle_milestone_load_last(const tangle_t *const tangle,
                                          iota_stor_pack_t *pack) {
  return iota_stor_milestone_load_last(&tangle->conn, pack);
}

retcode_t iota_tangle_milestone_load_next(const tangle_t *const tangle,
                                          uint64_t index,
                                          iota_stor_pack_t *pack) {
  return iota_stor_milestone_load_next(&tangle->conn, index, pack);
}

retcode_t iota_tangle_milestone_exist(const tangle_t *const tangle,
                                      flex_trit_t const *const hash,
                                      bool *exist) {
  return iota_stor_milestone_exist(&tangle->conn, hash, exist);
}

/*
 * Utilities
 */

retcode_t iota_tangle_find_tail(const tangle_t *const tangle,
                                trit_array_t *tx_hash, trit_array_t *tail,
                                bool *found_tail) {
  retcode_t res = RC_OK;
  struct _iota_transaction next_tx_s;
  iota_transaction_t next_tx = &next_tx_s;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  bool found_approver = false;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, tx_pack);

  *found_tail = false;

  res = iota_tangle_transaction_load(tangle, TRANSACTION_COL_HASH, tx_hash,
                                     &tx_pack);
  if (res != RC_OK || tx_pack.num_loaded == 0) {
    return res;
  }

  uint32_t index = curr_tx->current_index;
  memcpy(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243);

  iota_stor_pack_t hash_pack;
  if ((res = hash_pack_init(&hash_pack, 10)) != RC_OK) {
    return res;
  }

  while (res == RC_OK && index > 0 &&
         memcmp(curr_tx->bundle, bundle_hash, FLEX_TRIT_SIZE_243) == 0) {
    hash_pack_reset(&hash_pack);
    res = iota_tangle_transaction_load_hashes_of_approvers(
        tangle, curr_tx->hash, &hash_pack);

    if (res != RC_OK) {
      log_error(TANGLE_LOGGER_ID,
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
      res = iota_tangle_transaction_load(tangle, TRANSACTION_COL_HASH,
                                         approver_hash, &tx_pack);
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
    memcpy(tail->trits, curr_tx->hash, FLEX_TRIT_SIZE_243);
    *found_tail = true;
  }

  hash_pack_free(&hash_pack);

  return res;
}
