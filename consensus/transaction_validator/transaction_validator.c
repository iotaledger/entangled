/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/transaction_validator/transaction_validator.h"
#include "utils/logger_helper.h"

#define TRANSACTION_VALIDATOR_LOGGER_ID "consensus_transaction_validator"

retcode_t iota_consensus_transaction_validator_init(
    tangle_t* const tangle, transaction_validator_t* const tv) {
  logger_helper_init(TRANSACTION_VALIDATOR_LOGGER_ID, LOGGER_INFO, true);
  tv->tangle = tangle;
  return RC_OK;
}
