// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_GET_TRYTES_H
#define CCLIENT_RESPONSE_GET_TRYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  trit_array_array* trytes;
  size_t numTransactions;
} get_trytes_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRYTES_H
