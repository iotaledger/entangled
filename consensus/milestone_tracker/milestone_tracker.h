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

#ifdef __cplusplus
extern "C" {
#endif

// Foward declarations
typedef struct tangle_s tangle_t;
typedef struct _trit_array* trit_array_p;

typedef struct milestone_tracker_s {
  bool running;
  bool testnet;
  tangle_t* tangle;
  uint64_t milestone_start_index;
  thread_handle_t latest_milestone_tracker;
  uint64_t latest_milestone_index;
  trit_array_p latest_milestone;
  thread_handle_t solid_milestone_tracker;
  uint64_t latest_solid_subtangle_milestone_index;
  trit_array_p latest_solid_subtangle_milestone;
  trit_array_p coordinator;
  size_t num_keys_in_milestone;
  // bool accept_any_testnet_coo;
  // private final Set<Hash> analyzedMilestoneCandidates = new HashSet<>();
} milestone_tracker_t;

/**
 * Initializes a milestone tracker
 *
 * @param mt The milestone tracker
 * @param tangle A tangle
 * @param testnet Whether the node runs on testnet or not
 *
 * @return a status code
 */
extern retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt,
                                             tangle_t* const tangle,
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
