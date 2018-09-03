/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "were_addresses_spent_from.h"

were_addresses_spent_from_res_t* were_addresses_spent_from_res_new() {
  were_addresses_spent_from_res_t* addresses = NULL;
  utarray_new(addresses, &ut_str_icd);
  return addresses;
}

void were_addresses_spent_from_res_free(were_addresses_spent_from_res_t* res) {
  utarray_free(res);
}

char* were_addresses_spent_from_res_at(were_addresses_spent_from_res_t* in,
                                       int index) {
  if (utarray_len(in) > index) {
    char** p = NULL;
    p = (char**)utarray_eltptr(in, index);
    if (p) {
      return *p;
    }
  }
  return '\0';
}
