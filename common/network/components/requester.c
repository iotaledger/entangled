/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "requester.h"

bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash) {
  return state->queue->vtable->push(state->queue, hash) ==
         CONCURRENT_QUEUE_SUCCESS;
}

trit_array_p get_transaction_to_request(requester_state_t *const state) {
  trit_array_p hash;
  if (state->queue->vtable->pop(state->queue, &hash) !=
      CONCURRENT_QUEUE_SUCCESS)
    return NULL;
  return hash;
}
