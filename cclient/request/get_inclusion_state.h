/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_INCLUSION_STATE_H
#define CCLIENT_REQUEST_GET_INCLUSION_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * List of transactions you want to get the inclusion state for.
   */
  UT_array* hashes;
  /**
   * List of tips (including milestones) you want to search for the inclusion
   * state.
   */
  UT_array* tips;

} get_inclusion_state_req_t;

get_inclusion_state_req_t* get_inclusion_state_req_new();
void get_inclusion_state_req_free(get_inclusion_state_req_t** req);

void get_inclusion_state_req_add_hash(get_inclusion_state_req_t* req,
                                      char* hash);
void get_inclusion_state_req_add_tip(get_inclusion_state_req_t* req, char* tip);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_INCLUSION_STATE_H
