/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/api/conf.h"

retcode_t iota_api_conf_init(iota_api_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->port = DEFAULT_API_PORT;
  conf->max_get_trytes = DEFAULT_MAX_GET_TRYTES;

  return RC_OK;
}
