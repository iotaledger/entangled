// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_GET_BALANCES_H
#define CCLIENT_REQUEST_GET_BALANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * List of addresses you want to get the confirmed balance for.
   */
  address_array addresses;
  /**
   * Transactions with any of these tags will be returned
   */
  size_t threshold;

} get_balances_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_BALANCES_H