/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_tips.h"

get_tips_res_t* get_tips_res_new() {
  get_tips_res_t* hashes = NULL;
  utarray_new(hashes, &ut_str_icd);
  return hashes;
}

char* get_tips_res_hash_at(get_tips_res_t* hashes, int index) {
  if (utarray_len(hashes) > index) {
    char** p = NULL;
    p = (char**)utarray_eltptr(hashes, index);
    if (p) {
      return *p;
    }
  }
  return NULL;
}

int get_tips_res_hash_num(get_tips_res_t* hashes) {
  return utarray_len(hashes);
}

void get_tips_res_free(get_tips_res_t* ut) { utarray_free(ut); }
