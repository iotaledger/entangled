/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include "common/network/endpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

bool udp_send(endpoint_t *const endpoint);

#ifdef __cplusplus
}
#endif
