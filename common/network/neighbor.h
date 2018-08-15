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
#include "common/trinary/trit_array.h"

typedef struct {
  endpoint_t endpoint;
  unsigned int number_of_all_transactions;
  unsigned int number_of_new_transactions;
  unsigned int number_of_invalid_transactions;
  unsigned int number_of_sent_transactions;
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
retcode_t neighbor_send(neighbor_t *const neighbor, trit_array_p const hash);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_NEIGHBOR_H__
