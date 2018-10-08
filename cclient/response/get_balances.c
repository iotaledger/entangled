/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_balances.h"

get_balances_res_t* get_balances_res_new() {
  get_balances_res_t* res =
      (get_balances_res_t*)malloc(sizeof(get_balances_res_t));
  if (res) {
    utarray_new(res->balances, &ut_str_icd);
    res->milestone = flex_hash_array_new();
  }
  return res;
}

void get_balances_res_free(get_balances_res_t* res) {
  if (res) {
    utarray_free(res->balances);
    flex_hash_array_free(res->milestone);
    free(res);
  }
}

char* get_balances_res_balances_at(get_balances_res_t* in, int index) {
  char** p = (char**)utarray_eltptr(in->balances, index);
  return *p;
}

trit_array_p get_balances_res_milestone_at(get_balances_res_t* in, int index) {
  return flex_hash_array_at(in->milestone, index);
}
