/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_CHECK_CONSISTENCY_H
#define CCLIENT_REQUEST_CHECK_CONSISTENCY_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct check_consistency_req_s {
  /*
   * Tail transaction hash (hash of transaction with `currentIndex=0`), or array
   * of tail transaction hashes.
   */
  hash243_queue_t hashes;
} check_consistency_req_t;

check_consistency_req_t* check_consistency_req_new();
void check_consistency_req_free(check_consistency_req_t** req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_CHECK_CONSISTENCY_H
