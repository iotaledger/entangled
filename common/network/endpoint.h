/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_ENDPOINT_H__
#define __COMMON_NETWORK_ENDPOINT_H__

#include <stdint.h>

#define HOST_NAME_MAX 255

typedef struct {
  char host[HOST_NAME_MAX];
  uint16_t port;
} endpoint_t;

#endif  // __COMMON_NETWORK_ENDPOINT_H__
