/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_ENDPOINT_H__
#define __CIRI_NODE_NETWORK_ENDPOINT_H__

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ciri/node/network/network.h"

typedef struct endpoint_s {
  char domain[MAX_HOST_LENGTH];
  char ip[INET6_ADDRSTRLEN];
  uint16_t port;
  void *stream;
} endpoint_t;

int endpoint_cmp(endpoint_t const *const lhs, endpoint_t const *const rhs);

#endif  // __CIRI_NODE_NETWORK_ENDPOINT_H__
