// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_STORE_TRANSACTIONS_H
#define CCLIENT_REQUEST_STORE_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * Trunk transaction to store
   */
  trit_array_array* txs;
  size_t numTxs;

} store_transactions_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_STORE_TRANSACTIONS_H
