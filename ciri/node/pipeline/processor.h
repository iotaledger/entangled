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
#include "ciri/node/iota_packet.h"
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
typedef struct processor_s {
  thread_handle_t thread;
  bool running;
  iota_packet_queue_t queue;
  rw_lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
  transaction_validator_t *transaction_validator;
  transaction_solidifier_t *transaction_solidifier;
  milestone_tracker_t *milestone_tracker;
} processor_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a processor
 *
 * @param processor The processor state
 * @param node A node
 * @param transaction_validator A transaction validator
 * @param transaction_solidifier A transaction solidifier
 * @param milestone_tracker A milestone tracker
 *
 * @return a status code
 */
retcode_t processor_init(processor_t *const processor, node_t *const node,
                         transaction_validator_t *const transaction_validator,
                         transaction_solidifier_t *const transaction_solidifier,
                         milestone_tracker_t *const milestone_tracker);

/**
 * Starts a processor
 *
 * @param processor The processor state
 *
 * @return a status code
 */
retcode_t processor_start(processor_t *const processor);

/**
 * Stops a processor
 *
 * @param processor The processor state
 *
 * @return a status code
 */
retcode_t processor_stop(processor_t *const processor);

/**
 * Destroys a processor
 *
 * @param processor The processor state
 *
 * @return a status code
 */
retcode_t processor_destroy(processor_t *const processor);

/**
 * Adds a packet to a processor queue
 *
 * @param processor The processor state
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t processor_on_next(processor_t *const processor, iota_packet_t const packet);

/**
 * Gets the size of the processor queue
 *
 * @param processor The processor
 *
 * @return a status code
 */
size_t processor_size(processor_t *const processor);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_PIPELINE_PROCESSOR_H__
