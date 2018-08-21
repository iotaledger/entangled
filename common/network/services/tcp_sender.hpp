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

#ifdef __cplusplus
extern "C" {
#endif

bool tcp_send(endpoint_t *const endpoint, iota_packet_t *const packet);

#ifdef __cplusplus
}
#endif
