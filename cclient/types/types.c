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
// overwrite oom in utarray
#undef oom
#define oom() log_info(logger_id, "[%s:%d] OOM.\n", __func__, __LINE__)

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

static UT_icd ut_transactions_icd = {sizeof(iota_transaction_t), NULL, NULL, NULL};

transaction_array_t transaction_array_new() {
  transaction_array_t txs = NULL;
  utarray_new(txs, &ut_transactions_icd);
  return txs;
}

void transaction_array_push_back(transaction_array_t txs, iota_transaction_t const* const tx) {
  utarray_push_back(txs, tx);
}

size_t transaction_array_len(transaction_array_t txs) { return utarray_len(txs); }
void transaction_array_free(transaction_array_t txs) { utarray_free(txs); }

iota_transaction_t* transaction_array_at(transaction_array_t txs, size_t index) {
  // return NULL if not found.
  return (iota_transaction_t*)utarray_eltptr(txs, index);
}
