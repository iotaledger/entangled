/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "attach_to_tangle.h"

attach_to_tangle_res_t* attach_to_tangle_res_new() {
  attach_to_tangle_res_t* res = NULL;
  utarray_new(res, &ut_str_icd);
  return res;
}

const char* attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res,
                                           int index) {
  if (utarray_len(res) > index) {
    char** p = NULL;
    p = (char**)utarray_eltptr(res, index);
    if (p) {
      return *p;
    }
  }
  return NULL;
}

int attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res) {
  return utarray_len(res);
}

void attach_to_tangle_res_free(attach_to_tangle_res_t* ut) { utarray_free(ut); }