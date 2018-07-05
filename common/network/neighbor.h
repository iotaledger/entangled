/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_NEIGHBOR_H__
#define __COMMON_NETWORK_NEIGHBOR_H__

#include "common/network/iota_packet.h"

typedef enum connection_type_e { TCP, UDP } connection_type_t;

typedef struct {
  socket_addr_t endpoint;
  int socket;
  connection_type_t connection_type;
  unsigned int number_of_all_transactions;
  unsigned int number_of_new_transactions;
  unsigned int number_of_invalid_transactions;
  unsigned int number_of_sent_transactions;
  unsigned int random_transaction_requests;
  int flagged;
} neighbor_t;

int neighbor_send(neighbor_t *const neighbor,
                  iota_packet_t const *const packet);
int tcp_neighbor_send(neighbor_t *const neighbor,
                      iota_packet_t const *const packet);
int udp_neigbhor_send(neighbor_t *const neighbor,
                      iota_packet_t const *const packet);

#endif  // __COMMON_NETWORK_NEIGHBOR_H__
