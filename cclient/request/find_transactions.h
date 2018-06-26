// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_FIND_TRANSACTIONS_H
#define CCLIENT_REQUEST_FIND_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * Transactions with any of these addresses as input/output will be returned
   */
  address_array addresses;

  /**
   * Transactions with any of these tags will be returned
   */
  tag_array tags;

  /**
   * Transactions which directly approve any of approvees will be returned
   */
  address_array approvees;
  /**
   * Transactions belonging to bundles will be returned
   */
  bundle_hash_t* bundles;
  size_t numBundles;

} find_transactions_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H
