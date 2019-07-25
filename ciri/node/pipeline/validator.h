/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_VALIDATOR_H__
#define __CIRI_NODE_PIPELINE_VALIDATOR_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/handles/cond.h"
#include "utils/handles/lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct node_s node_t;
typedef struct transaction_validator_s transaction_validator_t;
typedef struct transaction_solidifier_s transaction_solidifier_t;
typedef struct milestone_tracker_s milestone_tracker_t;

typedef struct validator_stage_s {
  thread_handle_t thread;
  bool running;
  // protocol_gossip_queue_t queue;
  lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
  transaction_validator_t *transaction_validator;
  transaction_solidifier_t *transaction_solidifier;
  milestone_tracker_t *milestone_tracker;
} validator_stage_t;

/**
 * Initializes a validator stage
 *
 * @param validator The validator stage
 * @param node A node
 * @param transaction_validator A transaction validator
 * @param transaction_solidifier A transaction solidifier
 * @param milestone_tracker A milestone tracker
 *
 * @return a status code
 */
retcode_t validator_stage_init(validator_stage_t *const validator, node_t *const node,
                               transaction_validator_t *const transaction_validator,
                               transaction_solidifier_t *const transaction_solidifier,
                               milestone_tracker_t *const milestone_tracker);

/**
 * Starts a validator stage
 *
 * @param validator The validator stage
 *
 * @return a status code
 */
retcode_t validator_stage_start(validator_stage_t *const validator);

/**
 * Stops a validator stage
 *
 * @param validator The validator stage
 *
 * @return a status code
 */
retcode_t validator_stage_stop(validator_stage_t *const validator);

/**
 * Destroys a validator stage
 *
 * @param validator The validator stage
 *
 * @return a status code
 */
retcode_t validator_stage_destroy(validator_stage_t *const validator);

/**
 * Adds a packet to a validator stage queue
 *
 * @param validator The validator stage
 * @param packet The packet
 *
 * @return a status code
 */
// retcode_t validator_stage_add(validator_stage_t *const validator, protocol_gossip_t const *const packet);

/**
 * Gets the size of the validator stage queue
 *
 * @param validator The validator stage
 *
 * @return a status code
 */
size_t validator_stage_size(validator_stage_t *const validator);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_VALIDATOR_H__
