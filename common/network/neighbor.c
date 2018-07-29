/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/network/neighbor.h"
#include "common/network/uri_parser.h"

bool init_neighbor_with_uri(neighbor_t *const neighbor, char const *const uri) {
  char scheme[MAX_SCHEME_LENGTH];

  if (neighbor == NULL || uri == NULL) {
    return false;
  }
  memset(neighbor, 0, sizeof(neighbor_t));
  if (uri_parse(uri, scheme, neighbor->endpoint.host,
                &neighbor->endpoint.port) == false) {
    return false;
  }
  if (strcmp(scheme, "tcp") == 0) {
    neighbor->endpoint.protocol = ENDPOINT_PROTOCOL_TCP;
  } else if (strcmp(scheme, "udp") == 0) {
    neighbor->endpoint.protocol = ENDPOINT_PROTOCOL_UDP;
  } else {
    return false;
  }
  return true;
}

bool init_neighbor_with_values(neighbor_t *const neighbor,
                               protocol_type_t const protocol,
                               char const *const host, uint16_t const port) {
  if (neighbor == NULL || host == NULL) {
    return false;
  }
  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->endpoint.protocol = protocol;
  if (strlen(host) > MAX_HOST_LENGTH) {
    return false;
  }
  strcpy(neighbor->endpoint.host, host);
  neighbor->endpoint.port = port;
  return true;
}
