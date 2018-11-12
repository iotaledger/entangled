/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_INCLUSION_STATE_H
#define CCLIENT_REQUEST_GET_INCLUSION_STATE_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * List of transactions you want to get the inclusion state for.
   */
  hash243_queue_t hashes;
  /**
   * List of tips (including milestones) you want to search for the inclusion
   * state.
   */
  hash243_queue_t tips;

} get_inclusion_state_req_t;

get_inclusion_state_req_t* get_inclusion_state_req_new();
void get_inclusion_state_req_free(get_inclusion_state_req_t** req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_INCLUSION_STATE_H
