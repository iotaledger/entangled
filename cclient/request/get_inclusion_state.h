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
  tx_hash_array hashes;
  /**
   * List of tips (including milestones) you want to search for the inclusion
   * state.
   */
  tx_hash_array tips;

} get_inclusion_state_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_INCLUSION_STATE_H
