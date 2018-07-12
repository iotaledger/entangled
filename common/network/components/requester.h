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

typedef struct {
  requester_queue_t *queue;
} requester_state_t;

void request_transaction(requester_state_t *const state,
                         trit_array_t const *const hash);
trit_array_t *get_transaction_to_request();

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
