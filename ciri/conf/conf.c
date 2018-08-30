/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/conf/conf.h"
#include "ciri/conf/conf_values.h"

retcode_t ciri_conf_init(ciri_conf_t* conf) {
  if (conf == NULL) {
    return RC_CIRI_CONF_NULL_CONF;
  }

  memset(conf, 0, sizeof(ciri_conf_t));
  conf->api_port = CONF_DEFAULT_API_PORT;
  conf->neighbors = CONF_DEFAULT_NEIGHBORS;
  conf->conf_file = CONF_DEFAULT_CONF_FILE;
  conf->tcp_receiver_port = CONF_DEFAULT_TCP_RECEIVER_PORT;
  conf->udp_receiver_port = CONF_DEFAULT_UDP_RECEIVER_PORT;
  conf->testnet = CONF_DEFAULT_TESTNET;
  conf->remote = CONF_DEFAULT_REMOTE;
  conf->remote_auth_token = CONF_DEFAULT_REMOTE_AUTH_TOKEN;
  conf->remote_limit_api = CONF_DEFAULT_REMOTE_LIMIT_API;
  conf->send_limit = CONF_DEFAULT_SEND_LIMIT;
  conf->max_peers = CONF_DEFAULT_MAX_PEERS;
  conf->dns_resolution = CONF_DEFAULT_DNS_RESOLUTION;

  return RC_OK;
}
