/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/error.h"

error_res_t* error_res_new() {
  error_res_t* res = (error_res_t*)malloc(sizeof(error_res_t));

  if (res) {
    res->error = NULL;
  }
  return res;
}

retcode_t error_res_set(error_res_t* res, char const* const error) {
  if (!res->error) {
    res->error = char_buffer_new();
  }

  if (!res->error) {
    return RC_OOM;
  }

  char_buffer_set(res->error, error);
  return RC_OK;
}

void error_res_free(error_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->error) {
    char_buffer_free((*res)->error);
  }

  free(*res);
  *res = NULL;
}
