/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
#define CCLIENT_REQUEST_ATTACH_TO_TANGLE_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATTACH_TO_TANGLE_MAIN_MWM 14
#define ATTACH_TO_TANGLE_TEST_MWM 9

typedef struct {
  /**
   * Trunk transaction to approve
   */
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
  /**
   * branch transaction to approve
   */
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
  /**
   * Min Weight Magnitude,Proof of Work intensity. Minimum value is 18
   */
  uint8_t mwm;
  /**
   * List of trytes (raw transaction data) to attach to the tangle.
   */
  hash8019_array_p trytes;

} attach_to_tangle_req_t;

attach_to_tangle_req_t* attach_to_tangle_req_new();
void attach_to_tangle_req_free(attach_to_tangle_req_t** req);
void attach_to_tangle_req_init(attach_to_tangle_req_t* req,
                               flex_trit_t const* const trunk,
                               flex_trit_t const* const branch, uint8_t mwm);
void attach_to_tangle_req_add_trytes(attach_to_tangle_req_t* req,
                                     flex_trit_t const* const raw_trytes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
