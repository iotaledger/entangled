/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "check_consistency.h"

check_consistency_req_t* check_consistency_req_new() {
  check_consistency_req_t* tails = NULL;
  utarray_new(tails, &ut_str_icd);
  return tails;
}
void check_consistency_req_add(check_consistency_req_t* tails,
                               const char* tail) {
  utarray_push_back(tails, &tail);
}
void check_consistency_req_free(check_consistency_req_t* ut) {
  utarray_free(ut);
}