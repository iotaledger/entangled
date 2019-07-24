/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_PROCESSOR_H__
#define __CIRI_NODE_PIPELINE_PROCESSOR_H__

#include <stdbool.h>

#include "ciri/node/protocol/gossip.h"
#include "common/errors.h"
#include "utils/handles/cond.h"
#include "utils/handles/lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct node_s node_t;

/**
 * A processor is responsible for analyzing packets sent by neighbors.
 */
typedef struct processor_stage_s {
  thread_handle_t thread;
  bool running;
  protocol_gossip_queue_t queue;
  lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
} processor_stage_t;

/**
 * Initializes a processor stage
 *
 * @param processor The processor stage
 * @param node A node
 *
 * @return a status code
 */
retcode_t processor_stage_init(processor_stage_t *const processor, node_t *const node);

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
retcode_t processor_stage_add(processor_stage_t *const processor, protocol_gossip_t const *const packet);

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

#endif  //__CIRI_NODE_PIPELINE_PROCESSOR_H__
