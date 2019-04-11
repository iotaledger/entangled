/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "ciri/api/conf.h"

retcode_t iota_api_conf_init(iota_api_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->http_port = DEFAULT_API_HTTP_PORT;
  conf->max_find_transactions = DEFAULT_MAX_FIND_TRANSACTIONS;
  conf->max_get_trytes = DEFAULT_MAX_GET_TRYTES;
  memset(conf->remote_limit_api, 0, sizeof(conf->remote_limit_api));

  return RC_OK;
}

retcode_t iota_api_conf_destroy(iota_api_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  for (size_t i = 0; conf->remote_limit_api[i]; i++) {
    free(conf->remote_limit_api[i]);
  }

  return RC_OK;
}
