/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_CHECK_CONSISTENCY_H
#define CCLIENT_REQUEST_CHECK_CONSISTENCY_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct check_consistency_req_s {
  /*
   * Tail transaction hash (hash of transaction with `currentIndex=0`), or array
   * of tail transaction hashes.
   */
  hash243_queue_t tails;
} check_consistency_req_t;

check_consistency_req_t* check_consistency_req_new();
void check_consistency_req_free(check_consistency_req_t** req);
// add a tail to tails queue
static inline retcode_t check_consistency_req_tails_add(check_consistency_req_t* const req,
                                                        const flex_trit_t* const hash) {
  return hash243_queue_push(&req->tails, hash);
}
// get a tail from tails queue
static inline flex_trit_t* check_consistency_req_tails_get(check_consistency_req_t* const req, size_t index) {
  return hash243_queue_at(&req->tails, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_CHECK_CONSISTENCY_H
