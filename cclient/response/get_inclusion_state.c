/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_inclusion_state.h"

get_inclusion_state_res_t* get_inclusion_state_res_new() {
  get_inclusion_state_res_t* res =
      (get_inclusion_state_res_t*)malloc(sizeof(get_inclusion_state_res_t));
  if (res) {
    utarray_new(res->states, &ut_int_icd);
  }
  return res;
}

bool get_inclusion_state_res_bool_at(get_inclusion_state_res_t* in, int index) {
  int* b = (int*)utarray_eltptr(in->states, index);
  if (b != NULL) {
    return (*b > 0) ? true : false;
  }
  return false;
}

int get_inclusion_state_res_bool_num(get_inclusion_state_res_t* in) {
  return utarray_len(in->states);
}

void get_inclusion_state_res_free(get_inclusion_state_res_t** res) {
  if (*res) {
    utarray_free((*res)->states);
    free(*res);
    *res = NULL;
  }
}
