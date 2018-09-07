/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_SERVICES_RECEIVER_H__
#define __GOSSIP_SERVICES_RECEIVER_H__

#include <stdint.h>

#include "common/network/network.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct concurrent_queue_iota_packet_t_s receive_queue_t;
typedef struct receiver_state_s receiver_state_t;

typedef struct receiver_service_s {
  thread_handle_t thread;
  uint16_t port;
  protocol_type_t protocol;
  receiver_state_t* state;
  receive_queue_t* queue;
  void* context;
  void* opaque_socket;
} receiver_service_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Starts a receiver service
 *
 * @param service The receiver service
 *
 * @return a status code
 */
bool receiver_service_start(receiver_service_t* const service);

/**
 * Stops a receiver service
 *
 * @param service The receiver service
 *
 * @return a status code
 */
bool receiver_service_stop(receiver_service_t* const service);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_SERVICES_RECEIVER_H__
