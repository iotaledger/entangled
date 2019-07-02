/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_ENDPOINT_H__
#define __COMMON_NETWORK_ENDPOINT_H__

#include <stdint.h>

#include "common/network/network.h"

typedef struct endpoint_s {
  char host[MAX_HOST_LENGTH];
  char ip[MAX_HOST_LENGTH];
  uint16_t port;
  void *opaque_inetaddr;
} endpoint_t;

static inline bool endpoint_cmp(endpoint_t const *const lhs, endpoint_t const *const rhs) {
  return (lhs != NULL && rhs != NULL && (strcmp(lhs->ip, rhs->ip) == 0 || strcmp(lhs->host, rhs->host) == 0) &&
          lhs->port == rhs->port && lhs->protocol == rhs->protocol);
}

#endif  // __COMMON_NETWORK_ENDPOINT_H__
