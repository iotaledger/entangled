/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_CONF_H__
#define __CIRI_API_CONF_H__

#include <stdint.h>

#include "ciri/utils/files.h"
#include "common/errors.h"

#define API_ENDPOINTS_NUM 16

#define DEFAULT_API_HTTP_PORT 14265
#define DEFAULT_MAX_FIND_TRANSACTIONS 100000;
#define DEFAULT_MAX_GET_TRYTES 10000;

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate the
// IOTA API
typedef struct iota_api_conf_s {
  // HTTP API listen port
  uint16_t http_port;
  // The maximal number of transactions that may be returned by the
  // 'findTransactions' API call. If the number of transactions found exceeds
  // this number an error will be returned
  size_t max_find_transactions;
  // Maximum number of transactions that will be returned by the 'getTrytes' API
  // call
  size_t max_get_trytes;
  // Commands that should be ignored by API
  char* remote_limit_api[API_ENDPOINTS_NUM + 1];
  // Path of the spent addresses database file
  char spent_addresses_db_path[FILE_PATH_SIZE];
  // Path of the tangle database file
  char tangle_db_path[FILE_PATH_SIZE];
} iota_api_conf_t;

/**
 * Initializes API configuration with default values
 *
 * @param conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_api_conf_init(iota_api_conf_t* const conf);

/**
 * Destroys API configuration
 *
 * @param conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_api_conf_destroy(iota_api_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_API_CONF_H__
