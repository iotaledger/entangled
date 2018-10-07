/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/get_trytes.h"

get_trytes_req_t* get_trytes_req_new() { return flex_hash_array_new(); }
get_trytes_req_t* get_trytes_req_add(get_trytes_req_t* hashes,
                                     const char* hash) {
  return flex_hash_array_append(hashes, hash);
}
void get_trytes_req_free(get_trytes_req_t* hashes) {
  flex_hash_array_free(hashes);
}