/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_TYPES_H
#define CCLIENT_TYPES_H

#include <stdbool.h>
#include <stdlib.h>

#include "common/model/transaction.h"
#include "common/trinary/trit_array.h"
#include "common/trinary/tryte.h"

#include "common/errors.h"
#include "utarray.h"
#include "utlist.h"

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
  int_array* array;
} int_array_array;

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

typedef struct flex_hash_array {
  trit_array_p hash;  // trit_array_t is an array of flex_trit.
  struct flex_hash_array* next;
} flex_hash_array_t;

char_buffer_t* char_buffer_new();
retcode_t char_buffer_allocate(char_buffer_t* in, size_t n);
void char_buffer_free(char_buffer_t* in);

int_array* int_array_new();
retcode_t int_array_allocate(int_array* in, size_t n);
void int_array_free(int_array* in);

int_array_array* int_array_array_new();
retcode_t int_array_array_allocate(int_array_array* in, size_t n);
void int_array_array_free(int_array_array* in);

char* int_array_to_string(int_array* in);
int_array* string_to_int_array(char* in);

retcode_t flex_hash_to_trytes(trit_array_p hash, char* trytes);
retcode_t trytes_to_flex_hash(trit_array_p hash, const char* trytes);
retcode_t flex_hash_to_char_buffer(trit_array_p hash, char_buffer_t* out);

flex_hash_array_t* flex_hash_array_new();
flex_hash_array_t* flex_hash_array_append(flex_hash_array_t* head,
                                          const char* trytes);
trit_array_p flex_hash_array_at(flex_hash_array_t* head, size_t index);
int flex_hash_array_count(flex_hash_array_t* head);
void flex_hash_array_free(flex_hash_array_t* head);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_TYPES_H
