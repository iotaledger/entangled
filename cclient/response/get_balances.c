/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_balances.h"

get_balances_res_t* get_balances_res_new() {
  get_balances_res_t* res =
      (get_balances_res_t*)malloc(sizeof(get_balances_res_t));

  res->balances = int_array_array_new();
  res->milestone = flex_hash_array_new();
  return res;
}

void get_balances_res_free(get_balances_res_t* res) {
  if (res) {
    int_array_array_free(res->balances);
    flex_hash_array_free(res->milestone);
    free(res);
  }
}

char* get_balances_res_balances_at(get_balances_res_t* in, int index) {
  if (in->balances->size > index) {
    return int_array_to_string(in->balances->array + index);
  }
  return NULL;
}

trit_array_p get_balances_res_milestone_at(get_balances_res_t* in, int index) {
  return flex_hash_array_at(in->milestone, index);
}
