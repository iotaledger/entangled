/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/check_consistency.h"

check_consistency_res_t* check_consistency_res_new() {
  check_consistency_res_t* res = (check_consistency_res_t*)malloc(sizeof(check_consistency_res_t));

  if (res) {
    res->info = NULL;
    res->state = false;
  }
  return res;
}

retcode_t check_consistency_res_info_set(check_consistency_res_t* res, char const* const info) {
  if (!res->info) {
    res->info = char_buffer_new();
  }

  if (!res->info) {
    return RC_OOM;
  }

  char_buffer_set(res->info, info);
  return RC_OK;
}

void check_consistency_res_free(check_consistency_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->info) {
    char_buffer_free((*res)->info);
  }

  free(*res);
  *res = NULL;
}
