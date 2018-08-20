#include "consensus/trnasaction_validator/transaction_validator.h"

#define TRANSACTION_VALIDATOR_LOGGER_ID "consensus_transaction_validator"

extern retcode_t iota_consensus_transaction_validator_init(
    const tangle_t *tangle, transaction_validator_t *tv) {
  logger_helper_init(TRANSACTION_VALIDATOR_LOGGER_ID, LOGGER_INFO, true);
  tv->tangle = tangle;
}
