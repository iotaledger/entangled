/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_CONCURRENT_QUEUE_PACKET_H__
#define __COMMON_NETWORK_CONCURRENT_QUEUE_PACKET_H__

#include "common/network/iota_packet.h"
#include "common/network/queues/concurrent_queue.h.inc"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CONCURRENT_QUEUE_OF(iota_packet_t);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_CONCURRENT_QUEUE_PACKET_H__
