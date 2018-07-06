/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_RESPONDER_H__
#define __COMMON_NETWORK_COMPONENTS_RESPONDER_H__

#include "common/network/queues/concurrent_queue_hash_request.h"

typedef concurrent_queue_of_hash_request_t responder_queue_t;

typedef struct {
  responder_queue_t *queue;
} responder_state_t;

void responder_on_next(responder_state_t *const state,
                       trit_array_t const *const hash,
                       neighbor_t *const neighbor);
void *responder_routine(responder_state_t *const state);

#endif  //__COMMON_NETWORK_COMPONENTS_RESPONDER_H__
