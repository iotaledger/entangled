/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/types/types.h"

#define TYPES_LOGGER_ID "cclient/types"

static logger_id_t logger_id;

void logger_init_types() {
  logger_id = logger_helper_enable(TYPES_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__, TYPES_LOGGER_ID);
}

void logger_destroy_types() {
  log_info(logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__, TYPES_LOGGER_ID);
  logger_helper_release(logger_id);
}

char_buffer_t* char_buffer_new() {
  char_buffer_t* out = (char_buffer_t*)malloc(sizeof(char_buffer_t));
  if (out != NULL) {
    out->length = 0;
    out->data = NULL;
  } else {
    log_error(logger_id, "[%s:%d] %s \n", __func__, __LINE__, STR_CCLIENT_NULL_PTR);
  }
  return out;
}

retcode_t char_buffer_allocate(char_buffer_t* in, const size_t n) {
  if (in->length != 0) {
    return RC_OK;
  }
  in->data = (char*)malloc(sizeof(char) * (n + 1));
  if (in->data == NULL) {
    log_error(logger_id, "[%s:%d] %s \n", __func__, __LINE__, STR_CCLIENT_OOM);
    return RC_CCLIENT_OOM;
  }
  in->length = n;
  *(in->data + n) = '\0';
  return RC_OK;
}

retcode_t char_buffer_set(char_buffer_t* in, char const* const str) {
  size_t size = strlen(str);

  in->data = (char*)realloc(in->data, sizeof(char) * (size + 1));
  if (in->data == NULL) {
    log_error(logger_id, "[%s:%d] %s \n", __func__, __LINE__, STR_CCLIENT_OOM);
    return RC_CCLIENT_OOM;
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
