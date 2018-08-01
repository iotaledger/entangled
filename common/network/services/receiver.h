/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_SERVICES_RECEIVER_H__
#define __COMMON_NETWORK_SERVICES_RECEIVER_H__

#include "common/network/queues/concurrent_queue_packet.h"
#include "common/thread_handle.h"

typedef concurrent_queue_of_iota_packet_t receive_queue_t;
typedef struct receiver_state_s receiver_state_t;

typedef struct receiver_service_s {
  thread_handle_t thread;
  uint16_t port;
  protocol_type_t protocol;
  void* context;
  receiver_state_t* state;
  // TODO(thibault) remove ?
  receive_queue_t* queue;
} receiver_service_t;

#ifdef __cplusplus
extern "C" {
#endif

void receiver_service_prepare_packet(iota_packet_t* const packet,
                                     size_t const length,
                                     char const* const host,
                                     uint16_t const port,
                                     protocol_type_t const protocol);

bool receiver_service_start(receiver_service_t* const service);
bool receiver_service_stop(receiver_service_t* const service);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_SERVICES_RECEIVER_H__
