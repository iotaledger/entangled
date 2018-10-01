/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/check_consistency.h"

check_consistency_req_t* check_consistency_req_new() {
  return flex_hash_array_new();
}
flex_hash_array_t* check_consistency_req_add(check_consistency_req_t* tails,
                                             const char* tail) {
  return flex_hash_array_append(tails, tail);
}
void check_consistency_req_free(check_consistency_req_t* tails) {
  flex_hash_array_free(tails);
}
