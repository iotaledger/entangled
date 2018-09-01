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

int_array* int_array_new(){
  int_array* out = (int_array*)malloc(sizeof(int_array));
  out->size = 0;
  out->array = NULL;
  return out;
}

retcode_t int_array_allocate(int_array* in, size_t n) {
  if (in->size != 0) {
    return RC_OK;
  }
  in->array = (int*)malloc(sizeof(int) * (n + 1));
  if (in->array == NULL) return RC_CCLIENT_OOM;
  in->size = n;
  return RC_OK;
}

void int_array_free(int_array* in) {
  if (in) {
    if (in->array) free(in->array);
    free(in);
  }
}