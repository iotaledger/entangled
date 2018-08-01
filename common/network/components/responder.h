/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_RESPONDER_H__
#define __COMMON_NETWORK_COMPONENTS_RESPONDER_H__

#include "common/network/queues/concurrent_queue_hash_request.h"
#include "common/thread_handle.h"

typedef concurrent_queue_of_hash_request_t responder_queue_t;
typedef struct node_s node_t;

typedef struct responder_state_s {
  thread_handle_t thread;
  bool running;
  responder_queue_t *queue;
  node_t *node;
} responder_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool responder_init(responder_state_t *const state, node_t *const node);
bool responder_start(responder_state_t *const state);
bool responder_on_next(responder_state_t *const state, trit_array_p const hash,
                       neighbor_t *const neighbor);
bool responder_stop(responder_state_t *const state);
bool responder_destroy(responder_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_RESPONDER_H__
