/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_CONF_H__
#define __CIRI_API_CONF_H__

#include <stdint.h>

#include "common/errors.h"

#define DEFAULT_API_PORT 14265
#define DEFAULT_MAX_GET_TRYTES 10000;

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate the
// IOTA API
typedef struct iota_api_conf_s {
  // The port that will be used by the API
  uint16_t port;
  // The maximal number of trytes that may be returned by the "getTrytes" API
  // call. If the number of transactions found exceeds this number an error will
  // be returned.
  size_t max_get_trytes;
} iota_api_conf_t;

/**
 * Initializes API configuration with default values
 *
 * @param conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_api_conf_init(iota_api_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_API_CONF_H__
