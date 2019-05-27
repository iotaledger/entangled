/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/response/get_tips.h"

get_tips_res_t* get_tips_res_new() {
  get_tips_res_t* res = (get_tips_res_t*)malloc(sizeof(get_tips_res_t));
  if (res) {
    res->hashes = NULL;
  }
  return res;
}

size_t get_tips_res_hash_num(get_tips_res_t* res) { return hash243_stack_count(res->hashes); }

void get_tips_res_free(get_tips_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->hashes) {
    hash243_stack_free(&(*res)->hashes);
  }
  free(*res);
  *res = NULL;
}
