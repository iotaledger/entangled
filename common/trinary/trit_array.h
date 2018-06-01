/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_TRIT_ARRAY_H_
#define __COMMON_TRINARY_TRIT_ARRAY_H_

#include "trits.h"
#include "trit_byte.h"

typedef struct _trit_array *trit_array_t;
struct _trit_array {
  char *trits;
  size_t num_trits;
  size_t num_bytes;
  uint8_t dynamic;
};

size_t trit_array_bytes_for_trits(size_t num_trits);

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
trit_t trit_array_at(trit_array_t trit_array, size_t index);
void trit_array_set_at(trit_array_t trit_array, size_t index, trit_t trit);
void trit_array_set_trits(trit_array_t trit_array, char *trits, size_t num_trits);
trit_array_t trit_array_slice(trit_array_t trit_array, trit_array_t *to_trit_array, size_t start, size_t num_trits);

/***********************************************************************************************************
 * Constructor
 ***********************************************************************************************************/
// num_trits is number of trits
trit_array_t trit_array_new(size_t num_trits);

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
void trit_array_free(trit_array_t trit_array);

#define TRIT_ARRAY_DECLARE(NAME, NUM_TRITS) \
size_t NAME ## _num_trits = trit_array_bytes_for_trits(NUM_TRITS); \
char *NAME ## _trits[NAME ## _num_trits]; \
struct _trit_array NAME = { (char *) NAME ## _trits, NAME ## _num_trits };

#define TRIT_ARRAY_ASSIGN(NAME, NUM_TRITS, TRITS) \
size_t NAME ## _num_trits = trit_array_bytes_for_trits(NUM_TRITS); \
trit_t NAME ## _trits[] = {TRITS}; \
struct _trit_array NAME = { (char *) NAME ## _trits, NAME ## _num_trits };

#endif
