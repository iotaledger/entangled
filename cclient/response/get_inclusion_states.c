/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/get_inclusion_states.h"

get_inclusion_states_res_t* get_inclusion_states_res_new() {
  get_inclusion_states_res_t* res = (get_inclusion_states_res_t*)malloc(sizeof(get_inclusion_states_res_t));
  if (res) {
    res->states = NULL;
    utarray_new(res->states, &ut_int_icd);
  }
  return res;
}

retcode_t get_inclusion_states_res_states_set(get_inclusion_states_res_t* res, int st) {
  if (!res->states) {
    utarray_new(res->states, &ut_int_icd);
  }
  if (!res->states) {
    return RC_OOM;
  }
  utarray_push_back(res->states, &st);
  return RC_OK;
}

bool get_inclusion_states_res_states_at(get_inclusion_states_res_t* in, int index) {
  int* b = (int*)utarray_eltptr(in->states, index);
  if (b != NULL) {
    return (*b > 0) ? true : false;
  }
  return false;
}

int get_inclusion_states_res_states_count(get_inclusion_states_res_t* in) { return utarray_len(in->states); }

void get_inclusion_states_res_free(get_inclusion_states_res_t** res) {
  if (*res) {
    utarray_free((*res)->states);
    free(*res);
    *res = NULL;
  }
}
