/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_TYPES_H
#define CCLIENT_TYPES_H

#include <stdlib.h>

#include "common/trinary/trit_array.h"
#include "common/trinary/tryte.h"

#include "common/errors.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef oom
#define oom() printf("cclient oom\n")

#define NUM_TRITS_ADDRESS 243
#define NUM_TRITS_BUNDLE_HASH 243
#define NUM_TRITS_TAG 81
#define NUM_TRITS_TX_HASH 243

typedef trit_array_p address_t;
typedef trit_array_p bundle_hash_t;
typedef trit_array_p tag_t;
typedef trit_array_p tx_hash_t;

typedef struct {
  size_t length;
  char* data;
} char_buffer_t;

typedef struct {
  size_t size;
  int* array;
} int_array;

typedef struct {
  size_t size;
  trit_array_p* array;
} trit_array_array;

typedef struct {
  size_t size;
  address_t* array;
} address_array;

typedef struct {
  size_t size;
  tag_t* array;
} tag_array;

typedef struct {
  size_t size;
  tx_hash_t* array;
} tx_hash_array;

char_buffer_t* char_buffer_new();
retcode_t char_buffer_allocate(char_buffer_t* in, size_t n);
void char_buffer_free(char_buffer_t* in);

int_array* int_array_new();
retcode_t int_array_allocate(int_array* in, size_t n);
void int_array_free(int_array* in);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_TYPES_H
