/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/were_addresses_spent_from.h"

were_addresses_spent_from_res_t* were_addresses_spent_from_res_new() {
  were_addresses_spent_from_res_t* res =
      (were_addresses_spent_from_res_t*)malloc(sizeof(were_addresses_spent_from_res_t));
  if (res) {
    res->states = NULL;
  }
  return res;
}

retcode_t were_addresses_spent_from_res_states_add(were_addresses_spent_from_res_t* res, int state) {
  if (!res->states) {
    utarray_new(res->states, &ut_int_icd);
  }
  if (!res->states) {
    return RC_OOM;
  }
  utarray_push_back(res->states, &state);
  return RC_OK;
}

bool were_addresses_spent_from_res_states_at(were_addresses_spent_from_res_t* in, size_t index) {
  int* b = (int*)utarray_eltptr(in->states, index);
  if (b != NULL) {
    return (*b > 0) ? true : false;
  }
  return false;
}

size_t were_addresses_spent_from_res_states_count(were_addresses_spent_from_res_t* in) {
  if (in->states == NULL) {
    return 0;
  }
  return utarray_len(in->states);
}

void were_addresses_spent_from_res_free(were_addresses_spent_from_res_t** res) {
  if (*res) {
    if ((*res)->states) {
      utarray_free((*res)->states);
    }
    free(*res);
    *res = NULL;
  }
}
