/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_MILESTONE_TRACKER_MILESTONE_TRACKER_H__
#define __CONSENSUS_MILESTONE_TRACKER_MILESTONE_TRACKER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/handles/thread.h"

#define MILESTONE_TRACKER_INITIAL_HASH_PACK_SIZE 10
#define LATEST_MILESTONE_RESCAN_INTERVAL 5000
#define SOLID_MILESTONE_RESCAN_INTERVAL 5000

#ifdef __cplusplus
extern "C" {
#endif

// Foward declarations
typedef struct tangle_s tangle_t;
typedef struct snapshot_s snapshot_t;
typedef struct _trit_array* trit_array_p;
typedef struct ledger_validator_s ledger_validator_t;

typedef struct milestone_tracker_s {
  bool running;
  bool testnet;
  tangle_t* tangle;
  snapshot_t* latest_snapshot;
  uint64_t milestone_start_index;
  thread_handle_t latest_milestone_tracker;
  uint64_t latest_milestone_index;
  trit_array_p latest_milestone;
  thread_handle_t solid_milestone_tracker;
  uint64_t latest_solid_subtangle_milestone_index;
  trit_array_p latest_solid_subtangle_milestone;
  trit_array_p coordinator;
  size_t num_keys_in_milestone;
  ledger_validator_t* ledger_validator;
  // TODO
  // private final TransactionValidator transactionValidator;
  // private final MessageQ messageQ;
  // bool accept_any_testnet_coo;
} milestone_tracker_t;

/**
 * Initializes a milestone tracker
 *
 * @param mt The milestone tracker
 * @param tangle A tangle
 * @param snapshot An initial snapshot
 * @param lv A ledger validator
 * @param testnet Whether the node runs on testnet or not
 *
 * @return a status code
 */
extern retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt,
                                             tangle_t* const tangle,
                                             snapshot_t* const snapshot,
                                             ledger_validator_t* const lv,
                                             bool testnet);

/**
 * Starts a milestone tracker
 *
 * @param mt The milestone tracker
 *
 * @return a status code
 */
extern retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt);

/**
 * Stops a milestone tracker
 *
 * @param mt The milestone tracker
 *
 * @return a status code
 */
extern retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt);

/**
 * Destroys a milestone tracker
 *
 * @param mt The milestone tracker
 *
 * @return a status code
 */
extern retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_MILESTONE_TRACKER_MILESTONE_TRACKER_H__
