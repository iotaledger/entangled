/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_TRYTES_H
#define CCLIENT_REQUEST_GET_TRYTES_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_trytes_req_s {
  /* List of tx hashes for which request should get trytes */
  flex_hash_array_t* hashes;
} get_trytes_req_t;

get_trytes_req_t* get_trytes_req_new();
void get_trytes_req_free(get_trytes_req_t** const req);
get_trytes_req_t* get_trytes_req_add_hash(get_trytes_req_t* const req,
                                          tryte_t const* const hash);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_TRYTES_H
