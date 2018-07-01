#ifndef __COMMON_NETWORK_CONCURRENT_QUEUE_HASH_REQUEST_H__
#define __COMMON_NETWORK_CONCURRENT_QUEUE_HASH_REQUEST_H__

#include "common/model/neighbor.h"
#include "concurrent_queue.h.inc"

typedef struct {
  char *hash;
  neighbor_t neighbor;
} hash_request_t;

DECLARE_CONCURRENT_QUEUE_OF(hash_request_t);

#endif  // __COMMON_NETWORK_CONCURRENT_QUEUE_HASH_REQUEST_H__
