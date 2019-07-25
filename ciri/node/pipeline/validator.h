/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_VALIDATOR_H__
#define __CIRI_NODE_PIPELINE_VALIDATOR_H__

#include <stdbool.h>

#include "ciri/node/network/neighbor.h"
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

typedef struct validator_payload_s {
  protocol_gossip_queue_entry_t *gossip;
  uint64_t digest;
  neighbor_t *neighbor;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} validator_payload_t;

typedef struct validator_payload_queue_entry_s {
  validator_payload_t payload;
  struct validator_payload_queue_entry_s *next;
  struct validator_payload_queue_entry_s *prev;
} validator_payload_queue_entry_t;

typedef validator_payload_queue_entry_t *validator_payload_queue_t;

typedef struct validator_stage_s {
  thread_handle_t thread;
  bool running;
  validator_payload_queue_t queue;
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
 * @param[in]       gossip    A gossip packet
 * @param[in]       digest    The digest of the gossip transaction
 * @param[in]       neighbor  The neighbor that sent the packet
 *
 * @return a status code
 */
retcode_t validator_stage_add(validator_stage_t *const validator, protocol_gossip_queue_entry_t *const gossip,
                              uint64_t const digest, neighbor_t *const neighbor, flex_trit_t const *const hash);

/**
 * Gets the size of the validator stage queue
 *
 * @param validator The validator stage
 *
 * @return a status code
 */
size_t validator_stage_size(validator_stage_t *const validator);

/**
 * Gets the size of a validator payload queue
 *
 * @param[in] queue The validator payload queue
 *
 * @return the size of the packet queue
 */
size_t validator_payload_queue_count(validator_payload_queue_t const queue);

/**
 * Pushes a packet to a validator payload queue
 *
 * @param[out]  queue     The validator payload queue
 * @param[in]   gossip    A gossip packet
 * @param[in]   digest    The digest of the gossip transaction
 * @param[in]   neighbor  The neighbor that sent the packet
 *
 * @return a status code
 */
retcode_t validator_payload_queue_push(validator_payload_queue_t *const queue,
                                       protocol_gossip_queue_entry_t *const gossip, uint64_t const digest,
                                       neighbor_t *const neighbor, flex_trit_t const *const hash);

/**
 * Pops a packet from a validator payload queue
 *
 * @param[out]  queue The validator payload queue
 *
 * @return the validator payload
 */
validator_payload_queue_entry_t *validator_payload_queue_pop(validator_payload_queue_t *const queue);

/**
 * Frees a validator payload queue
 *
 * @param[out]  queue The validator payload queue
 *
 * @return a status code
 */
void validator_payload_queue_free(validator_payload_queue_t *const queue);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_VALIDATOR_H__
