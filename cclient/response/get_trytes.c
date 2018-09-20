/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_trytes.h"

get_trytes_res_t* get_trytes_res_new() {
  get_trytes_res_t* hashes = NULL;
  utarray_new(hashes, &ut_str_icd);
  return hashes;
}

char* get_trytes_res_at(get_trytes_res_t* trytes_array, int index) {
  if (utarray_len(trytes_array) > index) {
    char** p = NULL;
    p = (char**)utarray_eltptr(trytes_array, index);
    if (p) {
      return *p;
    }
  }
  return NULL;
}

int get_trytes_res_num(get_trytes_res_t* trytes_array) {
  return utarray_len(trytes_array);
}

void get_trytes_res_free(get_trytes_res_t* ut) { utarray_free(ut); }
