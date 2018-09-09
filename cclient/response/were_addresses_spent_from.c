/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "were_addresses_spent_from.h"

were_addresses_spent_from_res_t* were_addresses_spent_from_res_new() {
  were_addresses_spent_from_res_t* states = NULL;
  utarray_new(states, &ut_int_icd);
  return states;
}

void were_addresses_spent_from_res_free(were_addresses_spent_from_res_t* res) {
  utarray_free(res);
}

bool were_addresses_spent_from_res_at(were_addresses_spent_from_res_t* in,
                                      int index) {
  if (utarray_len(in) > index) {
    bool* p = NULL;
    p = (bool*)utarray_eltptr(in, index);
    if (p) {
      return *p;
    }
  }
  return NULL;
}

int were_addresses_spent_from_res_num(were_addresses_spent_from_res_t* in) {
  return utarray_len(in);
}
