/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
#define __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__

#include "utils/containers/queues/concurrent_queue_trit_array.h"
#include "utils/handles/thread.h"

typedef concurrent_queue_of_trit_array_p broadcaster_queue_t;
typedef struct node_s node_t;

typedef struct broadcaster_state_s {
  thread_handle_t thread;
  bool running;
  broadcaster_queue_t *queue;
  node_t *node;
} broadcaster_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool broadcaster_init(broadcaster_state_t *const state, node_t *const node);
bool broadcaster_start(broadcaster_state_t *const state);
bool broadcaster_on_next(broadcaster_state_t *const state,
                         trit_array_p const hash);
bool broadcaster_stop(broadcaster_state_t *const state);
bool broadcaster_destroy(broadcaster_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
