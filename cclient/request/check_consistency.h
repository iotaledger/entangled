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

/*
 * Tail transaction hash (hash of transaction with `currentIndex=0`), or array
 * of tail transaction hashes.
 */
typedef flex_hash_array_t check_consistency_req_t;

check_consistency_req_t* check_consistency_req_new();
flex_hash_array_t* check_consistency_req_add(check_consistency_req_t* tails,
                                             const char* tail);
void check_consistency_req_free(check_consistency_req_t* tails);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_CHECK_CONSISTENCY_H
