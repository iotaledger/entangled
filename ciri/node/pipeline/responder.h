/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_PIPELINE_RESPONDER_H__
#define __NODE_PIPELINE_RESPONDER_H__

#include <stdbool.h>

#include "ciri/node/protocol/gossip.h"
#include "ciri/node/protocol/transaction_request.h"
#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/handles/cond.h"
#include "utils/handles/lock.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct neighbor_s neighbor_t;
typedef struct node_s node_t;

/**
 * A responder stage is responsible for responding to transaction requests sent by neighbors.
 */
typedef struct responder_stage_s {
  thread_handle_t thread;
  bool running;
  transaction_request_queue_t queue;
  lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
} responder_stage_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a responder stage
 *
 * @param responder The responder stage
 * @param node A node
 *
 * @return a status code
 */
retcode_t responder_stage_init(responder_stage_t *const responder, node_t *const node);

/**
 * Starts a responder stage
 *
 * @param responder The responder stage
 *
 * @return a status code
 */
retcode_t responder_stage_start(responder_stage_t *const responder);

/**
 * Stops a responder stage
 *
 * @param responder The responder stage
 *
 * @return a status code
 */
retcode_t responder_stage_stop(responder_stage_t *const responder);

/**
 * Destroys a responder stage
 *
 * @param responder The responder stage
 *
 * @return a status code
 */
retcode_t responder_stage_destroy(responder_stage_t *const responder);

/**
 * Adds a request to a responder stage
 *
 * @param responder The responder stage
 * @param neighbor Requesting neighbor
 * @param hash Requested hash
 *
 * @return a status code
 */
retcode_t responder_stage_add(responder_stage_t *const responder, neighbor_t *const neighbor,
                              flex_trit_t const *const hash);

/**
 * Gets the size of the responder queue
 *
 * @param responder The responder stage
 *
 * @return a status code
 */
size_t responder_stage_size(responder_stage_t *const responder);

/**
 * Converts request bytes from a packet to a hash and adds it to the responder queue.
 *
 * @param responder The responder stage
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process request bytes
 * @param hash Transaction hash
 *
 * @return a status code
 */
retcode_t responder_process_request(responder_stage_t *const responder, neighbor_t *const neighbor,
                                    protocol_gossip_t const *const packet, flex_trit_t const *const hash);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_PIPELINE_RESPONDER_H__
