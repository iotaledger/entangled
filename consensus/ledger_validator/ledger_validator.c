#include "consensus/ledger_validator/ledger_validator.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

retcode_t iota_consensus_walker_validator_init(const tangle_t *tangle,
                                               const milestone_t *milestone,
                                               ledger_validator_t *lv) {
  logger_helper_init(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_INFO, true);
  lv->tangle = tangle;
  lv->milestone = milestone;
}

retcode_t iota_consensus_ledeger_validator_validate(
    const ledger_validator_t *lv, const tips_pair *pair) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}