/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/conf/conf.h"

retcode_t iota_ciri_conf_default(iota_ciri_conf_t *const ciri_conf,
                                 iota_consensus_conf_t *const consensus_conf,
                                 iota_gossip_conf_t *const gossip_conf,
                                 iota_api_conf_t *const api_conf) {
  retcode_t ret = RC_OK;

  if (ciri_conf == NULL || gossip_conf == NULL || consensus_conf == NULL) {
    return RC_NULL_PARAM;
  }

  memset(ciri_conf, 0, sizeof(iota_ciri_conf_t));

  ciri_conf->log_level = DEFAULT_LOG_LEVEL;

  if ((ret = iota_consensus_conf_init(consensus_conf)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_gossip_conf_init(gossip_conf)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_api_conf_init(api_conf)) != RC_OK) {
    return ret;
  }

  return ret;
}
