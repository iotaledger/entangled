/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_TCP_UTILS_H__
#define __CIRI_NODE_NETWORK_TCP_UTILS_H__

#include "ciri/node/network/neighbor.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Resolves a domain name into an IP address
 *
 * @param[in]   domain  The domain name
 * @param[out]  ip      The IP address
 *
 * @return a status code
 */
retcode_t tcp_resolve(char const *const domain, char *const ip);

/**
 * Initiates a connection with a neighbor
 *
 * @param[in] neighbor The neighbor
 *
 * @return a status code
 */
retcode_t tcp_connect(neighbor_t *const neighbor);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_NETWORK_TCP_UTILS_H__
