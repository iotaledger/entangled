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
#include "common/trinary/trit_array.h"

typedef struct {
  endpoint_t endpoint;
  unsigned int nbr_all_packets;
  unsigned int nbr_new_packets;
  unsigned int nbr_invalid_packets;
  unsigned int nbr_sent_packets;
  unsigned int random_transaction_requests;
  int flagged;
} neighbor_t;

#ifdef __cplusplus
extern "C" {
#endif

bool neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri);
bool neighbor_init_with_values(neighbor_t *const neighbor,
                               protocol_type_t const protocol,
                               char const *const host, uint16_t const port);
bool neighbor_send(neighbor_t *const neighbor, trit_array_p const hash);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_NEIGHBOR_H__
