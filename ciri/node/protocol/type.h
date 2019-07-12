/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PROTOCOL_PACKET_TYPE_H__
#define __CIRI_NODE_PROTOCOL_PACKET_TYPE_H__

#include <stdbool.h>

#include "ciri/node/protocol/gossip.h"
#include "ciri/node/protocol/handshake.h"
#include "ciri/node/protocol/header.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PACKET_TYPE_NBR 3

typedef enum packet_type_e { HEADER = 0, HANDSHAKE = 1, GOSSIP = 2 } packet_type_t;

typedef struct packet_type_desc_s {
  packet_type_t type;
  size_t max_length;
  bool dynamic_length;
} packet_type_desc_t;

static packet_type_desc_t packet_type_descs[PACKET_TYPE_NBR] = {{HEADER, HEADER_BYTES_LENGTH, false},
                                                                {HANDSHAKE, HANDSHAKE_MAX_BYTES_LENGTH, true},
                                                                {GOSSIP, GOSSIP_MAX_BYTES_LENGTH, true}};

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_PROTOCOL_PACKET_TYPE_H__
