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
 * Initializes an UDP endpoint
 *
 * @param endpoint The endpoint
 *
 * @return true on success, false otherwise
 */
bool udp_endpoint_init(endpoint_t *const endpoint);

/**
 * Destroys an UDP endpoint
 *
 * @param endpoint The endpoint
 *
 * @return true on success, false otherwise
 */
bool udp_endpoint_destroy(endpoint_t *const endpoint);

/**
 * Sends a UDP packet to an endpoint
 *
 * @param service An UDP service
 * @param endpoint The endpoint
 * @param packet The packet
 *
 * @return true if sending succeeded, false otherwise
 */
bool udp_send(receiver_service_t *const service, endpoint_t *const endpoint, iota_packet_t const *const packet);

#ifdef __cplusplus
}
#endif
