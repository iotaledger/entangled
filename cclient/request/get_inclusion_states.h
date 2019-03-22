/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_INCLUSION_STATES_H
#define CCLIENT_REQUEST_GET_INCLUSION_STATES_H

#include "cclient/types/types.h"

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

} get_inclusion_states_req_t;

get_inclusion_states_req_t* get_inclusion_states_req_new();
void get_inclusion_states_req_free(get_inclusion_states_req_t** req);

static inline retcode_t get_inclusion_states_req_hash_add(get_inclusion_states_req_t* const req,
                                                          flex_trit_t const* const hash) {
  return hash243_queue_push(&req->hashes, hash);
}
static inline flex_trit_t* get_inclusion_states_req_hash_get(get_inclusion_states_req_t* const req, size_t index) {
  return hash243_queue_at(&req->hashes, index);
}

static inline retcode_t get_inclusion_states_req_tip_add(get_inclusion_states_req_t* const req,
                                                         flex_trit_t const* const hash) {
  return hash243_queue_push(&req->tips, hash);
}
static inline flex_trit_t* get_inclusion_states_req_tip_get(get_inclusion_states_req_t* const req, size_t index) {
  return hash243_queue_at(&req->tips, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_INCLUSION_STATES_H
