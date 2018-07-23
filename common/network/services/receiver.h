/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_SERVICES_RECEIVER_H__
#define __COMMON_NETWORK_SERVICES_RECEIVER_H__

#include "common/network/components/receiver.h"

#ifdef __cplusplus
extern "C" {
#endif

void receiver_service_prepare_packet(iota_packet_t* const packet,
                                     size_t const length,
                                     char const* const host,
                                     uint16_t const port,
                                     protocol_type_t const protocol);
bool receiver_service_start(receiver_state_t* const state);
bool receiver_service_stop(receiver_state_t* const state);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_SERVICES_RECEIVER_H__
