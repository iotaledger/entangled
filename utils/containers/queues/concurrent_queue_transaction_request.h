/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_HASH_REQUEST_H__
#define __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_HASH_REQUEST_H__

#include "common/network/transaction_request.h"
#include "utils/containers//queues/concurrent_queue.h.inc"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CONCURRENT_QUEUE_OF(transaction_request_t);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_HASH_REQUEST_H__
