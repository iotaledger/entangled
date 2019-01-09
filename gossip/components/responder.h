/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_RESPONDER_H__
#define __GOSSIP_COMPONENTS_RESPONDER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "gossip/transaction_request.h"
#include "utils/containers/lock_free/lf_mpmc_queue_transaction_request_t.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct neighbor_s neighbor_t;
typedef struct node_s node_t;

/**
 * A responder is responsible for responding to transaction requests sent by
 * neighbors.
 */
typedef struct responder_s {
  thread_handle_t thread;
  bool running;
  lf_mpmc_queue_transaction_request_t_t queue;
  node_t *node;
} responder_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a responder
 *
 * @param responder The responder state
 * @param node A node
 *
 * @return a status code
 */
retcode_t responder_init(responder_t *const responder, node_t *const node);

/**
 * Starts a responder
 *
 * @param responder The responder state
 *
 * @return a status code
 */
retcode_t responder_start(responder_t *const responder);

/**
 * Stops a responder
 *
 * @param responder The responder state
 *
 * @return a status code
 */
retcode_t responder_stop(responder_t *const responder);

/**
 * Destroys a responder
 *
 * @param responder The responder state
 *
 * @return a status code
 */
retcode_t responder_destroy(responder_t *const responder);

/**
 * Adds a request to a responder
 *
 * @param responder The responder state
 * @param neighbor Requesting neighbor
 * @param hash Requested hash
 *
 * @return a status code
 */
retcode_t responder_on_next(responder_t *const responder,
                            neighbor_t *const neighbor,
                            flex_trit_t const *const hash);

/**
 * Gets the size of the responder queue
 *
 * @param responder The responder
 *
 * @return a status code
 */
static inline size_t responder_size(responder_t *const responder) {
  if (responder == NULL) {
    return 0;
  }

  return lf_mpmc_queue_transaction_request_t_count(&responder->queue);
}

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_RESPONDER_H__
