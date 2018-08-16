/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_NEIGHBOR_H__
#define __COMMON_NETWORK_NEIGHBOR_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/network/iota_packet.h"

typedef struct node_s node_t;

typedef struct {
  endpoint_t endpoint;
  unsigned int nbr_all_tx;
  unsigned int nbr_new_tx;
  unsigned int nbr_invalid_tx;
  unsigned int nbr_sent_tx;
  unsigned int random_transaction_requests;
  bool flagged;
} neighbor_t;

#ifdef __cplusplus
extern "C" {
#endif

retcode_t neighbor_init_with_uri(neighbor_t *const neighbor,
                                 char const *const uri);
retcode_t neighbor_init_with_values(neighbor_t *const neighbor,
                                    protocol_type_t const protocol,
                                    char const *const host,
                                    uint16_t const port);
retcode_t neighbor_send(node_t *const node, neighbor_t *const neighbor,
                        iota_packet_t *const packet);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_NEIGHBOR_H__
