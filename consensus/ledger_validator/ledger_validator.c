/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/ledger_validator/ledger_validator.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

retcode_t iota_consensus_walker_validator_init(tangle_t *const tangle,
                                               milestone_tracker_t *const mt,
                                               ledger_validator_t *const lv) {
  logger_helper_init(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  lv->tangle = tangle;
  lv->mt = mt;
  return RC_OK;
}

retcode_t iota_consensus_ledger_validator_destroy(
    ledger_validator_t *const lv) {
  logger_helper_destroy(LEDGER_VALIDATOR_LOGGER_ID);
  lv->tangle = NULL;
  return RC_OK;
}

retcode_t iota_consensus_ledeger_validator_validate(
    const ledger_validator_t *lv, const tips_pair *pair) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}
