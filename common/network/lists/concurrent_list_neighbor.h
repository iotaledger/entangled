/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_CONCURRENT_LIST_NEIGHBOR_H__
#define __COMMON_NETWORK_CONCURRENT_LIST_NEIGHBOR_H__

#include "common/network/lists/concurrent_list.h.inc"
#include "common/network/neighbor.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CONCURRENT_LIST_OF(neighbor_t);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_CONCURRENT_LIST_NEIGHBOR_H__
