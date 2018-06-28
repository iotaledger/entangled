// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
#define CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * Trunk transaction to approve
   */
  trit_array_array* txs;
  size_t numTxs;

} broadcast_transactions_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
