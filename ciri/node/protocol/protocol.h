/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PROTOCOL_PROTOCOL_H__
#define __CIRI_NODE_PROTOCOL_PROTOCOL_H__

#include "ciri/node/protocol/gossip.h"
#include "ciri/node/protocol/handshake.h"
#include "ciri/node/protocol/header.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PACKET_TYPES_NUM 3
#define PACKET_MAX_BYTES_LENGTH ((HEADER_BYTES_LENGTH) + (GOSSIP_MAX_BYTES_LENGTH))

typedef enum protocol_packet_type_e {
  PROTOCOL_HEADER = 0,
  PROTOCOL_HANDSHAKE = 1,
  PROTOCOL_GOSSIP = 2
} protocol_packet_type_t;

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_PROTOCOL_PROTOCOL_H__
