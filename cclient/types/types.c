/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "types.h"

char_buffer_t* char_buffer_new() {
  char_buffer_t* out = (char_buffer_t*)malloc(sizeof(char_buffer_t));
  out->length = 0;
  out->data = NULL;
  return out;
}

retcode_t char_buffer_allocate(char_buffer_t* in, size_t n) {
  if (in->length != 0) {
    return RC_OK;
  }
  in->data = (char*)malloc(sizeof(char) * (n + 1));
  if (in->data == NULL) return RC_CCLIENT_OOM;
  in->length = n;
  *(in->data + n) = '\0';
  return RC_OK;
}

void char_buffer_free(char_buffer_t* in) {
  if (in) {
    if (in->data) free(in->data);
    free(in);
  }
}
