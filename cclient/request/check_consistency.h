/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_CHECK_CONSISTENCY_H
#define CCLIENT_REQUEST_CHECK_CONSISTENCY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

/*
 * Tail transaction hash (hash of transaction with `currentIndex=0`), or array
 * of tail transaction hashes.
 */
typedef UT_array check_consistency_req_t;

check_consistency_req_t* check_consistency_req_new();
void check_consistency_req_add(check_consistency_req_t* tails,
                               const char* tail);
void check_consistency_req_free(check_consistency_req_t* ut);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_CHECK_CONSISTENCY_H
