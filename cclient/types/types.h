/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_TYPES_H
#define CCLIENT_TYPES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/trinary/trit_array.h"
#include "common/trinary/tryte.h"

#include "utils/containers/hash/hash243_queue.h"
#include "utils/containers/hash/hash81_queue.h"
#include "utils/logger_helper.h"

#include "utarray.h"
#include "utlist.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct flex_hash_array {
  trit_array_p hash;  // trit_array_t is an array of flex_trit.
  struct flex_hash_array* next;
} flex_hash_array_t;

void logger_init_types();
void logger_destroy_types();

char_buffer_t* char_buffer_new();
retcode_t char_buffer_allocate(char_buffer_t* in, const size_t n);
void char_buffer_free(char_buffer_t* in);

retcode_t flex_hash_to_trytes(const trit_array_p hash, char* trytes);
retcode_t trytes_to_flex_hash(trit_array_p hash, const char* trytes);
retcode_t flex_hash_to_char_buffer(trit_array_p hash, char_buffer_t* out);

flex_hash_array_t* flex_hash_array_new();
flex_hash_array_t* flex_hash_array_append(flex_hash_array_t* head,
                                          char const* const trytes);
trit_array_p flex_hash_array_at(flex_hash_array_t* head, size_t index);
int flex_hash_array_count(flex_hash_array_t* head);
void flex_hash_array_free(flex_hash_array_t* head);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_TYPES_H
