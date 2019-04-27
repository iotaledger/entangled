/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef UTILS_CHAR_BUFFER_H
#define UTILS_CHAR_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include "common/errors.h"
#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t length;
  char* data;
} char_buffer_t;

void logger_init_char_buffer();
void logger_destroy_char_buffer();

char_buffer_t* char_buffer_new();
retcode_t char_buffer_allocate(char_buffer_t* in, const size_t n);
retcode_t char_buffer_set(char_buffer_t* in, char const* const str);
void char_buffer_free(char_buffer_t* in);

#ifdef __cplusplus
}
#endif

#endif  // UTILS_CHAR_BUFFER_H
