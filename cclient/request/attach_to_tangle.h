/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
#define CCLIENT_REQUEST_ATTACH_TO_TANGLE_H

#include "types/types.h"

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
  int32_t mwm;
  /**
   * List of trytes (raw transaction data) to attach to the tangle.
   */
  hash8019_queue_t trytes;

} attach_to_tangle_req_t;

attach_to_tangle_req_t* attach_to_tangle_req_new();
void attach_to_tangle_req_free(attach_to_tangle_req_t** req);

retcode_t attach_to_tangle_req_set_trunk(attach_to_tangle_req_t* req,
                                         tryte_t const* const trunk);
retcode_t attach_to_tangle_req_set_branch(attach_to_tangle_req_t* req,
                                          tryte_t const* const branch);
void attach_to_tangle_req_set_mwm(attach_to_tangle_req_t* req,
                                  const int32_t mwm);
retcode_t attach_to_tangle_req_add_trytes(attach_to_tangle_req_t* req,
                                          tryte_t const* const trytes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
