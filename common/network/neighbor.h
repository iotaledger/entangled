/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_NEIGHBOR_H__
#define __COMMON_NETWORK_NEIGHBOR_H__

#include <stdbool.h>

#include "common/network/iota_packet.h"

typedef struct {
  endpoint_t endpoint;
  unsigned int number_of_all_transactions;
  unsigned int number_of_new_transactions;
  unsigned int number_of_invalid_transactions;
  unsigned int number_of_sent_transactions;
  unsigned int random_transaction_requests;
  int flagged;
} neighbor_t;

bool init_neighbor_with_uri(neighbor_t *const neighbor, char const *const uri);
bool init_neighbor_with_values(neighbor_t *const neighbor,
                               protocol_type_t const protocol,
                               char const *const host, uint16_t const port);

int neighbor_send(neighbor_t *const neighbor,
                  iota_packet_t const *const packet);
int tcp_neighbor_send(neighbor_t *const neighbor,
                      iota_packet_t const *const packet);
int udp_neigbhor_send(neighbor_t *const neighbor,
                      iota_packet_t const *const packet);

#endif  // __COMMON_NETWORK_NEIGHBOR_H__
