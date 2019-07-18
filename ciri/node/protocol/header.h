/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PROTOCOL_HEADER_H__
#define __CIRI_NODE_PROTOCOL_HEADER_H__

// The amount of bytes dedicated for the message type in the packet header
#define HEADER_TYPE_BYTES_LENGTH 1
// The amount of bytes dedicated for the message length denotation in the packet header
#define HEADER_LENGTH_BYTES_LENGTH 2
// The amount of bytes making up the packet header
#define HEADER_BYTES_LENGTH ((HEADER_TYPE_BYTES_LENGTH) + (HEADER_LENGTH_BYTES_LENGTH))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The message header sent in each message denoting the TLV fields
 */
typedef struct __attribute__((__packed__)) protocol_header_s {
  uint8_t type;
  uint16_t length;
} protocol_header_t;

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_PROTOCOL_HEADER_H__
