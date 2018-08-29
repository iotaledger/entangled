/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_trytes.h"

get_trytes_req_t* get_trytes_req_new() {
  get_trytes_req_t* hashes = NULL;
  utarray_new(hashes, &ut_str_icd);
  return hashes;
}
void get_trytes_req_add(get_trytes_req_t* hashes, const char* hash) {
  utarray_push_back(hashes, &hash);
}
void get_trytes_req_free(get_trytes_req_t* ut) { utarray_free(ut); }