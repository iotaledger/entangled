/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
#define __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__

#include "common/errors.h"
#include "consensus/conf.h"
#include "consensus/snapshot/snapshot.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/hash_maps.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct tangle_s tangle_t;
typedef struct milestone_tracker_s milestone_tracker_t;
typedef struct iota_milestone_s iota_milestone_t;
typedef struct requester_state_s requester_state_t;
typedef int8_t flex_trit_t;

typedef struct ledger_validator_s {
  iota_consensus_conf_t *conf;
  tangle_t *tangle;
  milestone_tracker_t *milestone_tracker;
  requester_state_t *transaction_requester;
} ledger_validator_t;

retcode_t iota_consensus_ledger_validator_init(
    ledger_validator_t *const lv, iota_consensus_conf_t *const conf,
    tangle_t *const tangle, milestone_tracker_t *const mt,
    requester_state_t *const tr);

retcode_t iota_consensus_ledger_validator_destroy(ledger_validator_t *const lv);

retcode_t iota_consensus_ledger_validator_update_snapshot(
    ledger_validator_t const *const lv, iota_milestone_t *const milestone,
    bool *const has_snapshot);

retcode_t iota_consensus_ledger_validator_check_consistency(
    ledger_validator_t const *const lv, hash243_stack_t const hashes,
    bool *const is_consistent);

retcode_t iota_consensus_ledger_validator_update_delta(
    ledger_validator_t const *const lv, hash243_set_t *const analyzed_hashes,
    state_delta_t *const delta, flex_trit_t const *const tip,
    bool *const is_consistent);

retcode_t iota_consensus_ledger_validator_destroy(ledger_validator_t *const lv);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
