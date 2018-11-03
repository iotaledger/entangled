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
    res->trytes = NULL;
  }
  return res;
}

void get_trytes_res_free(get_trytes_res_t** const res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->trytes) {
    hash243_queue_free(&(*res)->trytes);
  }
  free(*res);
  *res = NULL;
}

trit_array_p get_trytes_res_trytes_at(get_trytes_res_t const* const res,
                                      int index) {
  return hash243_queue_at(res->trytes, index);
}

get_trytes_res_t* get_trytes_res_add_trytes(get_trytes_res_t* const res,
                                            tryte_t const* const trytes) {
  hash243_queue_push(res->trytes, trytes);
  return res;
}

int get_trytes_res_trytes_num(get_trytes_res_t const* const res) {
  return hash243_queue_count(res->trytes);
}
