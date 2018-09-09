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

retcode_t were_addresses_spent_from_res_at(were_addresses_spent_from_res_t* res,
                                           int index, bool** states) {
  *states = NULL;
  if (utarray_len(res) > index) {
    *states = (bool*)malloc(sizeof(bool));
    *states = (bool*)utarray_eltptr(res, index);
    if (!(*states)) {
      return RC_CCLIENT_RES_ERROR;
    }
    return RC_OK;
  }
  return RC_CCLIENT_RES_ERROR;
}

int were_addresses_spent_from_res_num(were_addresses_spent_from_res_t* in) {
  return utarray_len(in);
}
