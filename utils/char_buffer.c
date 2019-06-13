/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "utils/char_buffer.h"

char_buffer_t* char_buffer_new() {
  char_buffer_t* out = (char_buffer_t*)malloc(sizeof(char_buffer_t));
  if (out != NULL) {
    out->length = 0;
    out->data = NULL;
  }
  return out;
}

retcode_t char_buffer_allocate(char_buffer_t* in, const size_t n) {
  if (in->length != 0) {
    return RC_OK;
  }
  in->data = (char*)malloc(sizeof(char) * (n + 1));
  if (in->data == NULL) {
    return RC_OOM;
  }
  in->length = n;
  *(in->data + n) = '\0';
  return RC_OK;
}

retcode_t char_buffer_set(char_buffer_t* in, char const* const str) {
  size_t size = strlen(str);

  if (in->data == NULL) {
    in->data = (char*)malloc(sizeof(char) * (size + 1));
  } else {
    in->data = (char*)realloc(in->data, sizeof(char) * (size + 1));
  }

  if (in->data == NULL) {
    return RC_OOM;
  }
  in->length = size;
  strcpy(in->data, str);
  return RC_OK;
}

void char_buffer_free(char_buffer_t* in) {
  if (in) {
    if (in->data) {
      free(in->data);
    }
    free(in);
  }
}
