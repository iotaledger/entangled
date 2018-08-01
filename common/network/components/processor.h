/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
#define __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__

#include "common/network/queues/concurrent_queue_packet.h"
#include "common/thread_handle.h"

typedef concurrent_queue_of_iota_packet_t processor_queue_t;
typedef struct node_s node_t;

typedef struct processor_state_s {
  thread_handle_t thread;
  bool running;
  processor_queue_t *queue;
  node_t *node;
} processor_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool processor_init(processor_state_t *const state, node_t *const node);
bool processor_start(processor_state_t *const state);
bool processor_on_next(processor_state_t *const state,
                       iota_packet_t const packet);
bool processor_stop(processor_state_t *const state);
bool processor_destroy(processor_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
