/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

// Forward declarations
typedef struct iota_packet_s iota_packet_t;
typedef struct endpoint_s endpoint_t;
typedef struct receiver_service_s receiver_service_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends a TCP packet to an endpoint
 *
 * @param endpoint The endpoint
 * @param packet The packet
 *
 * @return true if sending succeeded, false otherwise
 */
bool tcp_send(receiver_service_t *const service, endpoint_t *const endpoint,
              iota_packet_t *const packet);

#ifdef __cplusplus
}
#endif
