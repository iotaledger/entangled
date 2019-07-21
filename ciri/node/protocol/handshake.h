/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PROTOCOL_HANDSHAKE_H__
#define __CIRI_NODE_PROTOCOL_HANDSHAKE_H__

#include <stdint.h>

#include "common/trinary/bytes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HANDSHAKE_PORT_BYTES_LENGTH 2
#define HANDSHAKE_TIMESTAMP_BYTES_LENGTH 8
#define HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH 49
#define HANDSHAKE_MWM_BYTES_LENGTH 1
#define HANDSHAKE_SUPPORTED_VERSIONS_MIN_BYTES_LENGTH 1
#define HANDSHAKE_SUPPORTED_VERSIONS_MAX_BYTES_LENGTH 32
#define HANDSHAKE_MIN_BYTES_LENGTH                                                                                     \
  ((HANDSHAKE_PORT_BYTES_LENGTH) + (HANDSHAKE_TIMESTAMP_BYTES_LENGTH) + (HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH) + \
   (HANDSHAKE_MWM_BYTES_LENGTH) + (HANDSHAKE_SUPPORTED_VERSIONS_MIN_BYTES_LENGTH))
#define HANDSHAKE_MAX_BYTES_LENGTH                                                                                     \
  ((HANDSHAKE_PORT_BYTES_LENGTH) + (HANDSHAKE_TIMESTAMP_BYTES_LENGTH) + (HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH) + \
   (HANDSHAKE_MWM_BYTES_LENGTH) + (HANDSHAKE_SUPPORTED_VERSIONS_MAX_BYTES_LENGTH))

#define HANDSHAKE_PROTOCOL_VERSION = 1
#define HANDSHAKE_SUPPORTED_VERSIONS \
  { 0b00000001 }

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
typedef struct __attribute__((__packed__)) protocol_handshake_s {
  uint16_t port;
  uint64_t timestamp;
  uint8_t coordinator_address[HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH];
  uint8_t mwm;
  uint8_t supported_versions[HANDSHAKE_SUPPORTED_VERSIONS_MAX_BYTES_LENGTH];
} protocol_handshake_t;

/**
 * Initializes an handshake packet
 *
 * @param[out]  handshake           The handshake packet
 * @param[in]   port                The port used
 * @param[in]   coordinator_address The coordinator address used
 * @param[in]   mwm                 The MWM used
 * @param[out]  handshake_size      The size of the initialized handshake packet
 */
void handshake_init(protocol_handshake_t* const handshake, uint16_t const port, byte_t const* const coordinator_address,
                    uint8_t const mwm, uint16_t* const handshake_size);

/**
 * Returns the highest supported protocol version by the neighbor or a negative number indicating the highest
 * protocol version the neighbor would have supported but which our node doesn't.
 *
 * @param[in] supported_versions            The versions the neighbor supports
 * @param[in] supported_versions_length     The length of the versions the neighbor supports
 *
 * @return the highest supported protocol version
 */
int handshake_supported_version(uint8_t const* const supported_versions, size_t const supported_versions_length);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_PROTOCOL_HANDSHAKE_H__
