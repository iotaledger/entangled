// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
#define CCLIENT_RESPONSE_FIND_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  string_array transactions;
} find_transactions_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
