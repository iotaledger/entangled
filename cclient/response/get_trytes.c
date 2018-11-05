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

  get_trytes_res_t* tmp = *res;

  if (tmp->trytes) {
    hash8019_queue_free(&tmp->trytes);
  }
  free(tmp);
  *res = NULL;
}

flex_trit_t* get_trytes_res_trytes_at(get_trytes_res_t const* const res,
                                      int index) {
  return hash8019_queue_at(&res->trytes, index);
}

retcode_t get_trytes_res_add_trytes(get_trytes_res_t* const res,
                                    flex_trit_t const* const trytes) {
  return hash8019_queue_push(&res->trytes, trytes);
}

size_t get_trytes_res_trytes_num(get_trytes_res_t const* const res) {
  return hash8019_queue_count(&res->trytes);
}
