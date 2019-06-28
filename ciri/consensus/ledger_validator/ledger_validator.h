/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
#define __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__

#include "ciri/consensus/conf.h"
#include "ciri/consensus/snapshot/snapshot.h"
#include "common/errors.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/hash_indexed_map.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct tangle_s tangle_t;
typedef struct milestone_tracker_s milestone_tracker_t;
typedef struct iota_milestone_s iota_milestone_t;
typedef int8_t flex_trit_t;

typedef struct ledger_validator_s {
  iota_consensus_conf_t *conf;
  milestone_tracker_t *milestone_tracker;
} ledger_validator_t;

retcode_t iota_consensus_ledger_validator_init(ledger_validator_t *const lv, tangle_t const *const tangle,
                                               iota_consensus_conf_t *const conf, milestone_tracker_t *const mt);

retcode_t iota_consensus_ledger_validator_destroy(ledger_validator_t *const lv);

retcode_t iota_consensus_ledger_validator_update_snapshot(ledger_validator_t const *const lv, tangle_t *const tangle,
                                                          iota_milestone_t *const milestone, bool *const has_snapshot);

retcode_t iota_consensus_ledger_validator_check_consistency(ledger_validator_t const *const lv, tangle_t *const tangle,
                                                            hash243_stack_t const hashes, bool *const is_consistent);

retcode_t iota_consensus_ledger_validator_update_delta(ledger_validator_t const *const lv, tangle_t *const tangle,
                                                       hash243_set_t *const analyzed_hashes, state_delta_t *const delta,
                                                       flex_trit_t const *const tip, bool *const is_consistent);

retcode_t iota_consensus_ledger_validator_destroy(ledger_validator_t *const lv);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_LEDGER_VALIDATOR_LEDGER_VALIDATOR_H__
