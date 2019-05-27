/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CHAR_BUFFER_H__
#define __UTILS_CHAR_BUFFER_H__

#include <stdlib.h>

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t length;
  char* data;
} char_buffer_t;

char_buffer_t* char_buffer_new();
retcode_t char_buffer_allocate(char_buffer_t* in, const size_t n);
retcode_t char_buffer_set(char_buffer_t* in, char const* const str);
void char_buffer_free(char_buffer_t* in);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CHAR_BUFFER_H__
