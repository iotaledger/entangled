/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_extended
 *
 * @{
 *
 * @file
 * @brief
 *
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

/**
 * @brief Account information
 *
 */
typedef struct {
  uint64_t balance;                               /*!< total balance */
  flex_trit_t latest_address[FLEX_TRIT_SIZE_243]; /*!< unused address */
  hash243_queue_t addresses;                      /*!< List of used addresses */
  hash243_queue_t transactions;                   /*!< List of transactions */
} account_data_t;

/**
 * @brief Address genrating options
 *
 */
typedef struct {
  uint64_t start;   /*!< start key index */
  uint64_t total;   /*!< total key index */
  uint8_t security; /*!< security level */
} address_opt_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_EXTENDED_TYPES_H

/** @} */