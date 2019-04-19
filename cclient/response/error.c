/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/error.h"

error_res_t* error_res_new(char const* const error, uint16_t const status_code) {
  error_res_t* res = (error_res_t*)malloc(sizeof(error_res_t));

  if (res) {
    res->error = char_buffer_new();
    if (!res->error) {
      free(res);
      return NULL;
    }
    char_buffer_set(res->error, error);

    res->status_code = status_code;
  }

  return res;
}

char* error_res_get_message(error_res_t const* const res) {
  if (res == NULL || res->error == NULL) {
    return NULL;
  }
  return res->error->data;
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
