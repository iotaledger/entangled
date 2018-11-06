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
#include "utils/handles/thread.h"

// Forward declarations
typedef struct concurrent_queue_transaction_request_t_s responder_queue_t;
typedef struct neighbor_s neighbor_t;
typedef struct _trit_array *trit_array_p;
typedef struct node_s node_t;
typedef struct tangle_s tangle_t;

typedef struct responder_s {
  thread_handle_t thread;
  bool running;
  responder_queue_t *queue;
  node_t *node;
  tangle_t *tangle;
} responder_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a responder
 *
 * @param responder The responder state
 * @param node A node
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t responder_init(responder_t *const responder, node_t *const node,
                         tangle_t *const tangle);

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
                            trit_array_p const hash);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_RESPONDER_H__
