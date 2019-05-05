/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_MILESTONE_SERVICE_MILESTONE_SERVICE__
#define __CONSENSUS_MILESTONE_SERVICE_MILESTONE_SERVICE__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct milestone_service_s {
  iota_consensus_conf_t *conf;
} milestone_service_t;

/**
 * Initializes a snapshot
 *
 * @param milestone_service The service
 * @param conf The configuration
 *
 * @return a status code
 */
retcode_t iota_milestone_service_init(milestone_service_t *const milestone_service, iota_consensus_conf_t *conf);

/**
 * Destroys a snapshot
 *
 * @param milestone_service The service
 * @param tangle The tangle to load from
 *
 * @return a status code
 */
retcode_t iota_milestone_service_destroy(milestone_service_t *const milestone_service);

/**
 * Replays milestones on a snapshot
 *
 * @param milestone_service The service
 * @param snapshot The initial snapshot to forward with changes
 * @param index The last milestone in the new snapshot
 *
 * @return a status code
 */
retcode_t iota_milestone_service_replay_milestones(tangle_t *const tangle, milestone_service_t *const milestone_service,
                                                   snapshot_t *const snapshot, uint64_t index);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_MILESTONE_SERVICE_MILESTONE_SERVICE__
