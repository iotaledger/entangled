/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/consensus/transaction_validator/transaction_validator.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define TRANSACTION_VALIDATOR_LOGGER_ID "transaction_validator"

static logger_id_t logger_id;
static uint64_t MAX_TIMESTAMP_FUTURE_MS = 2 * 60 * 60 * 1000;

/*
 * Private functions
 */

/**
 * Checks if the timestamp of the transaction is below the last global snapshot
 * time or more than MAX_TIMESTAMP_FUTURE_MS milliseconds in the future, then
 * being invalid.
 * First, the attachment timestamp (set after performing PoW) is checked, and
 * if not available the regular timestamp is checked.
 * Genesis transaction will always be valid.
 *
 * @param tv Transaction validator
 * @param transaction Transaction under test
 *
 * @returns true if timestamp is invalid, false otherwise
 */
static bool has_invalid_timestamp(transaction_validator_t const* const tv,
                                  iota_transaction_t const* const transaction) {
  uint64_t timestamp_ms = transaction_attachment_timestamp(transaction) == 0
                              ? transaction_timestamp(transaction) * 1000UL
                              : transaction_attachment_timestamp(transaction);
  bool is_requested = false;
  bool is_too_futuristic = timestamp_ms > (current_timestamp_ms() + MAX_TIMESTAMP_FUTURE_MS);
  bool is_below_snapshot =
      (timestamp_ms < tv->snapshots_provider->inital_snapshot.metadata.timestamp * 1000UL) &&
      !iota_snapshot_has_solid_entry_point(&tv->snapshots_provider->inital_snapshot, transaction_hash(transaction));

  requester_is_requested(tv->transaction_requester, transaction_hash(transaction), true, &is_requested);

  if (is_requested) {
    return false;
  }

  if (is_too_futuristic) {
    return true;
  }

  if (is_below_snapshot) {
    return memcmp(transaction_hash(transaction), tv->conf->genesis_hash, FLEX_TRIT_SIZE_243) != 0;
  }

  return false;
}

/*
 * Public functions
 */

retcode_t iota_consensus_transaction_validator_init(transaction_validator_t* const tv,
                                                    snapshots_provider_t* const snapshots_provider,
                                                    transaction_requester_t* const transaction_requester,
                                                    iota_consensus_conf_t* const conf) {
  logger_id = logger_helper_enable(TRANSACTION_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  tv->conf = conf;
  tv->snapshots_provider = snapshots_provider;
  tv->transaction_requester = transaction_requester;

  return RC_OK;
}

retcode_t iota_consensus_transaction_validator_destroy(transaction_validator_t* const tv) {
  tv->conf = NULL;
  logger_helper_release(logger_id);

  return RC_OK;
}

bool iota_consensus_transaction_validate(transaction_validator_t const* const tv,
                                         iota_transaction_t const* const transaction) {
  if (transaction_weight_magnitude(transaction) < tv->conf->mwm) {
    log_debug(logger_id, "Validation failed: insufficient transaction weight\n");
    return false;
  }

  if (has_invalid_timestamp(tv, transaction)) {
    log_debug(logger_id, "Validation failed: invalid timestamp\n");
    return false;
  }

  if (llabs(transaction_value(transaction)) > IOTA_SUPPLY) {
    log_debug(logger_id, "Validation failed: invalid value\n");
    return false;
  }

  if (transaction_value(transaction) != 0 &&
      flex_trits_at(transaction_address(transaction), NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS - 1) != 0) {
    log_debug(logger_id, "Validation failed: invalid address for value transaction\n");
    return false;
  }

  return true;
}
