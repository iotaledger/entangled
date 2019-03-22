/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_MILESTONE_TRACKER_MILESTONE_TRACKER_H__
#define __CONSENSUS_MILESTONE_TRACKER_MILESTONE_TRACKER_H__

#include <stdbool.h>

#include "common/crypto/sponge/sponge.h"
#include "common/errors.h"
#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "consensus/conf.h"
#include "utils/containers/hash/hash243_queue.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

// Foward declarations
typedef struct tangle_s tangle_t;
typedef struct snapshot_s snapshot_t;
typedef struct _trit_array* trit_array_p;
typedef struct ledger_validator_s ledger_validator_t;
typedef struct transaction_solidifier_s transaction_solidifier_t;

typedef enum milestone_status_e {
  MILESTONE_VALID,
  MILESTONE_INVALID,
  MILESTONE_EXISTS,
  MILESTONE_INCOMPLETE,
} milestone_status_t;

typedef struct milestone_tracker_s {
  bool running;
  iota_consensus_conf_t* conf;
  snapshot_t* latest_snapshot;
  uint64_t milestone_start_index;
  thread_handle_t milestone_validator;
  uint64_t latest_milestone_index;
  flex_trit_t latest_milestone[FLEX_TRIT_SIZE_243];
  thread_handle_t milestone_solidifier;
  uint64_t latest_solid_subtangle_milestone_index;
  flex_trit_t latest_solid_subtangle_milestone[FLEX_TRIT_SIZE_243];
  ledger_validator_t* ledger_validator;
  transaction_solidifier_t* transaction_solidifier;
  hash243_queue_t candidates;
  rw_lock_handle_t candidates_lock;
  // bool accept_any_testnet_coo;
} milestone_tracker_t;

/**
 * Initializes a milestone tracker
 *
 * @param mt The milestone tracker
 * @param conf Consensus configuration
 * @param snapshot An initial snapshot
 * @param lv A ledger validator
 *
 * @return a status code
 */
retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt, iota_consensus_conf_t* const conf,
                                      snapshot_t* const snapshot, ledger_validator_t* const lv,
                                      transaction_solidifier_t* ts);

/**
 * Starts a milestone tracker
 *
 * @param mt The milestone tracker
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt, tangle_t* const tangle);

/**
 * Stops a milestone tracker
 *
 * @param mt The milestone tracker
 *
 * @return a status code
 */
retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt);

/**
 * Destroys a milestone tracker
 *
 * @param mt The milestone tracker
 *
 * @return a status code
 */
retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt);

/**
 * Pushes a milestone candidate to the milestone tracker candidates queue
 *
 * @param mt The milestone tracker
 * @param hash The candidate hash
 *
 * @return a status code
 */
retcode_t iota_milestone_tracker_add_candidate(milestone_tracker_t* const mt, flex_trit_t const* const hash);

uint64_t iota_milestone_tracker_get_milestone_index(iota_transaction_t* const tx);

retcode_t iota_milestone_tracker_validate_milestone(milestone_tracker_t* const mt, tangle_t* const tangle,
                                                    iota_milestone_t* const candidate,
                                                    milestone_status_t* const milestone_status);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_MILESTONE_TRACKER_MILESTONE_TRACKER_H__
