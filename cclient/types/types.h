// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_TYPES_H
#define CCLIENT_TYPES_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  size_t size;
  int* array;
} int_array;

typedef struct {
  size_t size;
  char** array;
} string_array;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_TYPES_H
