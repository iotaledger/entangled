/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_CONF_H__
#define __GOSSIP_CONF_H__

#include <stdint.h>

#include "common/errors.h"

// Network conf

#define DEFAULT_UDP_RECEIVER_PORT 14600
#define DEFAULT_TCP_RECEIVER_PORT 15600

#define PACKET_TX_SIZE 1604
#define PACKET_SIZE PACKET_TX_SIZE + REQUEST_HASH_SIZE
#define DEFAULT_MWN MWM

// Node conf

#define DEFAULT_NEIGHBORS NULL

// Probabilities conf

#define DEFAULT_PROBABILITY_SELECT_MILESTONE_CHILD 0.7
#define DEFAULT_PROBABILITY_REMOVE_REQUEST 0.01

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_gossip_conf_s {
  // Network conf
  uint16_t udp_receiver_port;
  uint16_t tcp_receiver_port;
  uint8_t mwm;
  // Node conf
  char* neighbors;
  // Probabilities conf
  double p_select_milestone_child;
  double p_remove_request;
} iota_gossip_conf_t;

retcode_t iota_gossip_conf_init(iota_gossip_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_CONF_H__
