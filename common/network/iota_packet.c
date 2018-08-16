/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/network/iota_packet.h"

void iota_packet_build(iota_packet_t* const packet, size_t const length,
                       char const* const host, uint16_t const port,
                       protocol_type_t const protocol) {
  if (packet == NULL) {
    return;
  }
  packet->length = length;
  if (host != NULL) {
    strcpy(packet->source.host, host);
  }
  packet->source.port = port;
  packet->source.protocol = protocol;
}
