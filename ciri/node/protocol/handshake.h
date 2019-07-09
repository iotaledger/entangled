/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PROTOCOL_HANDSHAKE_H__
#define __CIRI_NODE_PROTOCOL_HANDSHAKE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HANDSHAKE_PORT_BYTES_LENGTH 2
#define HANDSHAKE_TIMESTAMP_BYTES_LENGTH 8
#define HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH 49
#define HANDSHAKE_MWM_BYTES_LENGTH 1
#define HANDSHAKE_SUPPORTED_VERSIONS_BYTES_LENGTH 32
#define HANDSHAKE_BYTES_LENGTH                                                                                         \
  ((HANDSHAKE_PORT_BYTES_LENGTH) + (HANDSHAKE_TIMESTAMP_BYTES_LENGTH) + (HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH) + \
   (HANDSHAKE_MWM_BYTES_LENGTH) + (HANDSHAKE_SUPPORTED_VERSIONS_BYTES_LENGTH))

#define HANDSHAKE_PROTOCOL_VERSION = 1
#define HANDSHAKE_SUPPORTED_PROTOCOL_VERSIONS = {0b00000001}

typedef enum protocol_handshake_state_e {
  HANDSHAKE_INIT,
  HANDSHAKE_FAILED,
  HANDSHAKE_OK,
} protocol_handshake_state_t;

/**
 * The initial handshake packet sent over the wire up on a new neighbor connection.
 * Made up of:
 * - own server socket port (2 bytes)
 * - time at which the packet was sent (8 bytes)
 * - own used byte encoded coordinator address (49 bytes)
 * - own used MWM (1 byte)
 * - supported protocol versions. we need up to 32 bytes to represent 256 possible protocol versions. only up to N bytes
 * are used to communicate the highest supported version.
 */
typedef struct protocol_handshake_s {
  uint16_t port;
  uint64_t timestamp;
  uint8_t coordinator_address[HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH];
  uint8_t mwm;
  uint8_t supported_versions[HANDSHAKE_SUPPORTED_VERSIONS_BYTES_LENGTH];
} protocol_handshake_t;

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_PROTOCOL_HANDSHAKE_H__
