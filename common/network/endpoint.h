/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_ENDPOINT_H__
#define __COMMON_NETWORK_ENDPOINT_H__

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "common/network/network.h"

typedef struct endpoint_s {
  char domain[MAX_HOST_LENGTH];
  char ip[INET6_ADDRSTRLEN];
  uint16_t port;
} endpoint_t;

static inline bool endpoint_cmp(endpoint_t const *const lhs, endpoint_t const *const rhs) {
  return (lhs != NULL && rhs != NULL && (strcmp(lhs->ip, rhs->ip) == 0 || strcmp(lhs->domain, rhs->domain) == 0) &&
          lhs->port == rhs->port);
}

#endif  // __COMMON_NETWORK_ENDPOINT_H__
