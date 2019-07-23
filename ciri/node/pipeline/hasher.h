/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_HASHER_H__
#define __CIRI_NODE_PIPELINE_HASHER_H__

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
typedef struct transaction_validator_s transaction_validator_t;
typedef struct transaction_solidifier_s transaction_solidifier_t;
typedef struct milestone_tracker_s milestone_tracker_t;

typedef struct hasher_payload_s {
  protocol_gossip_queue_entry_t *gossip;
  uint64_t digest;
} hasher_payload_t;

typedef struct hasher_payload_queue_entry_s {
  hasher_payload_t payload;
  struct hasher_payload_queue_entry_s *next;
  struct hasher_payload_queue_entry_s *prev;
} hasher_payload_queue_entry_t;

typedef hasher_payload_queue_entry_t *hasher_payload_queue_t;

typedef struct hasher_stage_s {
  thread_handle_t thread;
  bool running;
  hasher_payload_queue_t queue;
  lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
  transaction_validator_t *transaction_validator;
  transaction_solidifier_t *transaction_solidifier;
  milestone_tracker_t *milestone_tracker;
} hasher_stage_t;

/**
 * Initializes a hasher stage
 *
 * @param[in, out]  hasher  The hasher stage
 * @param node A node
 * @param transaction_validator A transaction validator
 * @param transaction_solidifier A transaction solidifier
 * @param milestone_tracker A milestone tracker
 *
 * @return a status code
 */
retcode_t hasher_stage_init(hasher_stage_t *const hasher, node_t *const node,
                            transaction_validator_t *const transaction_validator,
                            transaction_solidifier_t *const transaction_solidifier,
                            milestone_tracker_t *const milestone_tracker);

/**
 * Starts a hasher stage
 *
 * @param[in, out]  hasher  The hasher stage
 *
 * @return a status code
 */
retcode_t hasher_stage_start(hasher_stage_t *const hasher);

/**
 * Stops a hasher stage
 *
 * @param[in, out]  hasher  The hasher stage
 *
 * @return a status code
 */
retcode_t hasher_stage_stop(hasher_stage_t *const hasher);

/**
 * Destroys a hasher stage
 *
 * @param[in, out]  hasher  The hasher stage
 *
 * @return a status code
 */
retcode_t hasher_stage_destroy(hasher_stage_t *const hasher);

/**
 * Adds a payload to a hasher stage queue
 *
 * @param[in, out]  hasher  The hasher stage
 * @param[in]       gossip  A gossip packet
 * @param[in]       digest  The digest of the gossip transaction
 *
 * @return a status code
 */
retcode_t hasher_stage_add(hasher_stage_t *const hasher, protocol_gossip_queue_entry_t *const gossip,
                           uint64_t const digest);

/**
 * Gets the size of the hasher stage queue
 *
 * @param[in, out]  hasher  The hasher stage
 *
 * @return a status code
 */
size_t hasher_stage_size(hasher_stage_t *const hasher);

/**
 * Gets the size of a hasher payload queue
 *
 * @param[in] queue The hasher payload queue
 *
 * @return the size of the packet queue
 */
size_t hasher_payload_queue_count(hasher_payload_queue_t const queue);

/**
 * Pushes a packet to a hasher payload queue
 *
 * @param[out]  queue   The hasher payload queue
 * @param[in]   gossip  A gossip packet
 * @param[in]   digest  The digest of the gossip transaction
 *
 * @return a status code
 */
retcode_t hasher_payload_queue_push(hasher_payload_queue_t *const queue, protocol_gossip_queue_entry_t *const gossip,
                                    uint64_t const digest);

/**
 * Pops a packet from a hasher payload queue
 *
 * @param[out]  queue The hasher payload queue
 *
 * @return the hasher payload
 */
hasher_payload_queue_entry_t *hasher_payload_queue_pop(hasher_payload_queue_t *const queue);

/**
 * Frees a hasher payload queue
 *
 * @param[out]  queue The hasher payload queue
 *
 * @return a status code
 */
void hasher_payload_queue_free(hasher_payload_queue_t *const queue);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_HASHER_H__
