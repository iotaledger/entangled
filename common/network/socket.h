/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_SOCKET_H__
#define __COMMON_NETWORK_SOCKET_H__

#include <stdint.h>

typedef struct {
  char *host;
  uint16_t port;
} socket_addr_t;

#endif  // __COMMON_NETWORK_SOCKET_H__
