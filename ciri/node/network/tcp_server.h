/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_TCP_SERVER_H__
#define __CIRI_NODE_NETWORK_TCP_SERVER_H__

#include <stdbool.h>

#include <uv.h>

#include "ciri/node/protocol/type.h"
#include "common/errors.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct tcp_server_s {
  // Metadata
  bool running;
  thread_handle_t thread;
  // Data
  node_t *node;
} tcp_server_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a TCP server
 *
 * @param[out]  tcp_server  The TCP server
 * @param[in]   node        A node
 *
 * @return a status code
 */
retcode_t tcp_server_init(tcp_server_t *const tcp_server, node_t *const node);

/**
 * Starts a TCP server
 *
 * @param[out]  tcp_server  The TCP server
 *
 * @return a status code
 */
retcode_t tcp_server_start(tcp_server_t *const tcp_server);

/**
 * Stops a TCP server
 *
 * @param[out]  tcp_server  The TCP server
 *
 * @return a status code
 */
retcode_t tcp_server_stop(tcp_server_t *const tcp_server);

/**
 * Destroys a TCP server
 *
 * @param[out]  tcp_server  The TCP server
 *
 * @return a status code
 */
retcode_t tcp_server_destroy(tcp_server_t *const tcp_server);

/**
 * Resolves a domain name into an IP address
 *
 * @param[in]   domain  The domain name
 * @param[out]  ip      The IP address
 *
 * @return a status code
 */
retcode_t tcp_server_resolve_domain(char const *const domain, char *const ip);

/**
 * Initiates a connection with a neighbor
 *
 * @param[in] neighbor The neighbor
 *
 * @return a status code
 */
retcode_t tcp_server_connect(neighbor_t *const neighbor);

/**
 * Writes data to a stream
 *
 * @param[in] stream      The stream
 * @param[in] type        The packet type
 * @param[in] buffer      The buffer
 * @param[in] buffer_size The buffer size
 *
 * @return a status code
 */
retcode_t tcp_server_write(uv_stream_t *const stream, packet_type_t const type, void *const buffer,
                           uint16_t const buffer_size);

void tcp_server_on_async_write(uv_async_t *const handle);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_NETWORK_TCP_SERVER_H__
