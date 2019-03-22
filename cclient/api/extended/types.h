/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_EXTENDED_TYPES_H
#define CCLIENT_API_EXTENDED_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t total_balance;
  hash243_queue_t addresses;
} inputs_t;

typedef struct {
  size_t balance;
  flex_trit_t latest_address[FLEX_TRIT_SIZE_243];
  hash243_queue_t addresses;
  hash243_queue_t transactions;
} account_data_t;

typedef struct {
  size_t security;
  size_t start;
  size_t total;
} address_opt_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_EXTENDED_TYPES_H