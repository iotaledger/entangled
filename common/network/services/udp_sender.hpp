/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include "common/network/iota_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

bool udp_send(endpoint_t *const endpoint, iota_packet_t *const packet);

#ifdef __cplusplus
}
#endif
