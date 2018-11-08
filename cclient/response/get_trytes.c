/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_trytes.h"

get_trytes_res_t* get_trytes_res_new() {
  get_trytes_res_t* res = (get_trytes_res_t*)malloc(sizeof(get_trytes_res_t));
  if (res) {
    res->trytes = flex_hash_array_new();
  }
  return res;
}

void get_trytes_res_free(get_trytes_res_t** const res) {
  if (!res || !(*res)) {
    return;
  }

  get_trytes_res_t* tmp = *res;

  if (tmp->trytes) {
    flex_hash_array_free(tmp->trytes);
  }
  free(tmp);
  *res = NULL;
}

trit_array_p get_trytes_res_trytes_at(get_trytes_res_t const* const res,
                                      int index) {
  return flex_hash_array_at(res->trytes, index);
}

get_trytes_res_t* get_trytes_res_add_trytes(get_trytes_res_t* const res,
                                            tryte_t const* const trytes) {
  res->trytes = flex_hash_array_append(res->trytes, (char*)trytes);
  return res;
}

int get_trytes_res_trytes_num(get_trytes_res_t const* const res) {
  return flex_hash_array_count(res->trytes);
}
