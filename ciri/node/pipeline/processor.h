/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_PIPELINE_PROCESSOR_H__
#define __NODE_PIPELINE_PROCESSOR_H__

#include <stdbool.h>

#include "ciri/consensus/transaction_validator/transaction_validator.h"
#include "ciri/node/protocol/gossip.h"
#include "common/errors.h"
#include "utils/handles/cond.h"
#include "utils/handles/lock.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;
typedef struct tangle_s tangle_t;
typedef struct transaction_solidifier_s transaction_solidifier_t;
typedef struct milestone_tracker_s milestone_tracker_t;

/**
 * A processor is responsible for analyzing packets sent by neighbors.
 */
typedef struct processor_stage_s {
  thread_handle_t thread;
  bool running;
  protocol_gossip_queue_t queue;
  rw_lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
  transaction_validator_t *transaction_validator;
  transaction_solidifier_t *transaction_solidifier;
  milestone_tracker_t *milestone_tracker;
} processor_stage_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a processor stage
 *
 * @param processor The processor stage
 * @param node A node
 * @param transaction_validator A transaction validator
 * @param transaction_solidifier A transaction solidifier
 * @param milestone_tracker A milestone tracker
 *
 * @return a status code
 */
retcode_t processor_stage_init(processor_stage_t *const processor, node_t *const node,
                               transaction_validator_t *const transaction_validator,
                               transaction_solidifier_t *const transaction_solidifier,
                               milestone_tracker_t *const milestone_tracker);

/**
 * Starts a processor stage
 *
 * @param processor The processor stage
 *
 * @return a status code
 */
retcode_t processor_stage_start(processor_stage_t *const processor);

/**
 * Stops a processor stage
 *
 * @param processor The processor stage
 *
 * @return a status code
 */
retcode_t processor_stage_stop(processor_stage_t *const processor);

/**
 * Destroys a processor stage
 *
 * @param processor The processor stage
 *
 * @return a status code
 */
retcode_t processor_stage_destroy(processor_stage_t *const processor);

/**
 * Adds a packet to a processor stage queue
 *
 * @param processor The processor stage
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t processor_stage_add(processor_stage_t *const processor, protocol_gossip_t const packet);

/**
 * Gets the size of the processor stage queue
 *
 * @param processor The processor stage
 *
 * @return a status code
 */
size_t processor_stage_size(processor_stage_t *const processor);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_PIPELINE_PROCESSOR_H__
