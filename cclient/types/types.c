// Copyright 2018 IOTA Foundation

#include "types.h"

char_buffer* char_buffer_new() {
  char_buffer* out = malloc(sizeof(char_buffer));
  out->length = 0;
  out->data = NULL;
  return out;
}

retcode_t char_buffer_allocate(char_buffer* in, size_t n) {
  if (in->length != 0) {
    return RC_OK;
  }
  in->data = (char*)malloc(sizeof(char) * (n + 1));
  if (in->data == NULL) return RC_CCLIENT_OOM;
  in->length = n;
  memset(in->data, '\0', 1);
  return RC_OK;
}

void char_buffer_free(char_buffer* in) {
  if (in) {
    if (in->data) free(in->data);
    free(in);
  }
}
