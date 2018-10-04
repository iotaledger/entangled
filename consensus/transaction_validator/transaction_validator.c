/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/transaction_validator/transaction_validator.h"
#include "consensus/defs.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#include "math.h"

#define TRANSACTION_VALIDATOR_LOGGER_ID "consensus_transaction_validator"

static size_t MAX_TIMESTAMP_FUTURE_MS = 2 * 60 * 60 * 1000;

static flex_trit_t genesis_hash[FLEX_TRIT_SIZE_243];

static bool has_invalid_timestamp(transaction_validator_t* const tv,
                                  const iota_transaction_t const transaction) {
  uint64_t max_future_timestamp_ms =
      current_timestamp_ms() + MAX_TIMESTAMP_FUTURE_MS;
  if (transaction->attachment_timestamp == 0) {
    return (transaction->timestamp < tv->snapshot_timestamp_ms &&
            memcmp(genesis_hash, transaction->hash, FLEX_TRIT_SIZE_243)) ||
           transaction->timestamp > max_future_timestamp_ms;
  }

  return transaction->attachment_timestamp < tv->snapshot_timestamp_ms ||
         transaction->attachment_timestamp >
             tv->snapshot_timestamp_ms + MAX_TIMESTAMP_FUTURE_MS;
}

retcode_t iota_consensus_transaction_validator_init(
    transaction_validator_t* const tv, uint64_t snapshot_timestamp_ms,
    uint8_t mwm) {
  logger_helper_init(TRANSACTION_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  tv->snapshot_timestamp_ms = snapshot_timestamp_ms;
  tv->mwm = mwm;
  memset(genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  return RC_OK;
}

retcode_t iota_consensus_transaction_validator_destroy(
    transaction_validator_t* const tv) {
  logger_helper_destroy(TRANSACTION_VALIDATOR_LOGGER_ID);
  return RC_OK;
}

bool iota_consensus_transaction_validate(
    transaction_validator_t* const tv,
    const iota_transaction_t const transaction) {
  if (transaction_weight_magnitude(transaction) < tv->mwm) {
    log_error(TRANSACTION_VALIDATOR_LOGGER_ID,
              "Validation failed, Invalid hash\n");
    return false;
  }

  if (has_invalid_timestamp(tv, transaction)) {
    log_error(TRANSACTION_VALIDATOR_LOGGER_ID,
              "Validation failed, Invalid timestamp\n");
    return false;
  }
  if (llabs(transaction->value) > IOTA_SUPPLY) {
    log_error(TRANSACTION_VALIDATOR_LOGGER_ID,
              "Validation failed, Invalid value\n");
    return false;
  }

  trit_t buffer[NUM_TRITS_FOR_FLEX_TRIT];
  flex_trits_to_trits(buffer, NUM_TRITS_FOR_FLEX_TRIT,
                      &transaction->address[FLEX_TRIT_SIZE_243 - 1],
                      NUM_TRITS_FOR_FLEX_TRIT, NUM_TRITS_FOR_FLEX_TRIT);
  if (transaction->value != 0 && buffer[NUM_TRITS_FOR_FLEX_TRIT - 1] != 0) {
    log_error(TRANSACTION_VALIDATOR_LOGGER_ID,
              "Validation failed, Invalid address for value transaction\n");
    return false;
  }

  return true;
}
