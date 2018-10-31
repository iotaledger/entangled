/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/conf/conf.h"
#include "ciri/conf/conf_values.h"

retcode_t iota_ciri_conf_default(iota_ciri_conf_t *const ciri_conf,
                                 iota_consensus_conf_t *const consensus_conf,
                                 iota_gossip_conf_t *const gossip_conf,
                                 iota_api_conf_t *const api_conf) {
  retcode_t ret = RC_OK;

  if (ciri_conf == NULL || gossip_conf == NULL || consensus_conf == NULL) {
    return RC_NULL_PARAM;
  }

  memset(ciri_conf, 0, sizeof(iota_ciri_conf_t));

  ciri_conf->log_level = CONF_DEFAULT_LOG_LEVEL;
  ciri_conf->api_port = CONF_DEFAULT_API_PORT;
  ciri_conf->conf_file = CONF_DEFAULT_CONF_FILE;
  ciri_conf->remote = CONF_DEFAULT_REMOTE;
  ciri_conf->remote_auth_token = CONF_DEFAULT_REMOTE_AUTH_TOKEN;
  ciri_conf->remote_limit_api = CONF_DEFAULT_REMOTE_LIMIT_API;
  ciri_conf->send_limit = CONF_DEFAULT_SEND_LIMIT;
  ciri_conf->max_peers = CONF_DEFAULT_MAX_PEERS;
  ciri_conf->dns_resolution = CONF_DEFAULT_DNS_RESOLUTION;

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
