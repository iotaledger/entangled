/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
#define __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__

#include "common/errors.h"

// Forward declarations
typedef struct tangle_s tangle_t;
typedef struct milestone_tracker_s milestone_tracker_t;
typedef struct requester_state_s requester_state_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ledger_validator_s {
  tangle_t *tangle;
  milestone_tracker_t *milestone_tracker;
  requester_state_t *transaction_requester;
} ledger_validator_t;

extern retcode_t iota_consensus_ledger_validator_init(
    ledger_validator_t *const lv, tangle_t *const tangle,
    milestone_tracker_t *const mt, requester_state_t *const tr);

extern retcode_t iota_consensus_ledger_validator_destroy(
    ledger_validator_t *const lv);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
