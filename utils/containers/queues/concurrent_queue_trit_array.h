/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_CONCURRENT_QUEUE_TRIT_ARRAY_H__
#define __COMMON_NETWORK_CONCURRENT_QUEUE_TRIT_ARRAY_H__

#include "common/network/queues/concurrent_queue.h.inc"
#include "common/trinary/trit_array.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CONCURRENT_QUEUE_OF(trit_array_p);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_CONCURRENT_QUEUE_TRIT_ARRAY_H__
