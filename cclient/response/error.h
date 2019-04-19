/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_ERROR_H
#define CCLIENT_RESPONSE_ERROR_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct error_res_s {
  char_buffer_t* error;
  uint16_t status_code;
} error_res_t;

error_res_t* error_res_new(char const* const error);
char* error_res_get_message(error_res_t const* const res);
void error_res_free(error_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ERROR_H
