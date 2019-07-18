/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "ciri/node/network/endpoint.h"

int endpoint_cmp(endpoint_t const *const lhs, endpoint_t const *const rhs) {
  int ip_cmp = 0;

  if (lhs == NULL || rhs == NULL) {
    return -1;
  }

  if (lhs->port != rhs->port) {
    return lhs->port - rhs->port;
  }

  if ((ip_cmp = strcmp(lhs->ip, rhs->ip)) == 0 || strcmp(lhs->domain, rhs->domain) == 0) {
    return 0;
  }

  return ip_cmp;
}
