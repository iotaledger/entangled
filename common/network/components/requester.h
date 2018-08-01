/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_REQUESTER_H__
#define __COMMON_NETWORK_COMPONENTS_REQUESTER_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

typedef concurrent_queue_of_trit_array_p requester_queue_t;
typedef struct node_s node_t;

typedef struct requester_state_s {
  requester_queue_t *queue;
  node_t *node;
} requester_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool requester_init(requester_state_t *const state, node_t *const node);
bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash);
trit_array_p get_transaction_to_request(requester_state_t *const state);
bool requester_destroy(requester_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
