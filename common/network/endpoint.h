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

typedef struct {
  char host[HOST_NAME_MAX];
  uint16_t port;
  protocol_type_t protocol;
} endpoint_t;

#endif  // __COMMON_NETWORK_ENDPOINT_H__
