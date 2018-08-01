/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_IOTA_PACKET_H__
#define __COMMON_NETWORK_IOTA_PACKET_H__

#include <stdlib.h>

#include "common/network/endpoint.h"

#define TRANSACTION_PACKET_SIZE 1650

typedef struct {
  char content[TRANSACTION_PACKET_SIZE + 1];
  size_t length;
  endpoint_t source;
} iota_packet_t;

#endif  // __COMMON_NETWORK_IOTA_PACKET_H__
