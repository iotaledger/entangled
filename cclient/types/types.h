// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_TYPES_H
#define CCLIENT_TYPES_H

#include <stdlib.h>

#include "common/trinary/trit_array.h"
#include "common/trinary/tryte.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_TRITS_ADDRESS 243
#define NUM_TRITS_BUNDLE_HASH 243
#define NUM_TRITS_TAG 81
#define NUM_TRITS_TX_HASH 243

typedef trit_array_p address_t;
typedef trit_array_p bundle_hash_t;
typedef trit_array_p tag_t;
typedef trit_array_p tx_hash_t;

typedef struct {
  size_t size;
  int* array;
} int_array;

typedef struct {
  size_t size;
  trit_array_p* array;
} tryte_array;

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

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_TYPES_H
