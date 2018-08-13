// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_GET_TRYTES_H
#define CCLIENT_REQUEST_GET_TRYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * List of tx hashes for which request should get trytes
   */
  flex_trit_array hashes;

} get_trytes_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_TRYTES_H
