/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_HASH_REQUEST_H__
#define __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_HASH_REQUEST_H__

#include "common/network/neighbor.h"
#include "common/trinary/trit_array.h"
#include "utils/containers//queues/concurrent_queue.h.inc"

typedef struct {
  trit_array_p hash;
  neighbor_t neighbor;
} hash_request_t;

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CQ(hash_request_t);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_HASH_REQUEST_H__
