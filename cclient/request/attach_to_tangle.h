// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
#define CCLIENT_REQUEST_ATTACH_TO_TANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * Trunk transaction to approve
   */
  tx_hash_t trunk;
  /**
   * branch transaction to approve
   */
  tx_hash_t branch;
  /**
   * Proof of Work intensity.
   */
  int weightMagnitude;
  /**
   * Transaction's raw trytes
   */
  trit_array_array txToAttachTrytes;

} attach_to_tangle_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
