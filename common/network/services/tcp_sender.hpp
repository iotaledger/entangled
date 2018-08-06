/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include "common/network/endpoint.h"
#include "common/trinary/trit_array.h"

#ifdef __cplusplus
extern "C" {
#endif

bool tcp_send(endpoint_t *const endpoint, trit_array_p const hash);

#ifdef __cplusplus
}
#endif
