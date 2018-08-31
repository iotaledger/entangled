/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
#define CCLIENT_REQUEST_ATTACH_TO_TANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * Trunk transaction to approve
   */
  char_buffer_t* trunk;
  /**
   * branch transaction to approve
   */
  char_buffer_t* branch;
  /**
   * Min Weight Magnitude,Proof of Work intensity. Minimum value is 18
   */
  int mwm;
  /**
   * List of trytes (raw transaction data) to attach to the tangle.
   */
  UT_array* trytes;

} attach_to_tangle_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
