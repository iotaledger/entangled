/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/network/tcp_server.h"
#include "ciri/node/network/tcp_utils.h"
#include "utils/logger_helper.h"

#define TCP_SERVER_LOGGER_ID "tcp_server"

static logger_id_t logger_id;

retcode_t tcp_resolve(char const *const domain, char *const ip) {
  struct addrinfo hints;
  uv_getaddrinfo_t resolver;
  int err = 0;
  retcode_t ret = RC_OK;

  logger_id = logger_helper_enable(TCP_SERVER_LOGGER_ID, LOGGER_DEBUG, true);

  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  if ((err = uv_getaddrinfo(uv_default_loop(), &resolver, NULL, domain, NULL, &hints)) != 0) {
    log_error(logger_id, "Can't resolve domain: %s\n", uv_err_name(err));
    return RC_DOMAIN_RESOLUTION;
  }

  switch (resolver.addrinfo->ai_family) {
    case AF_INET:
      if ((err = uv_ip4_name((struct sockaddr_in *)resolver.addrinfo->ai_addr, ip, INET_ADDRSTRLEN)) != 0) {
        log_error(logger_id, "Can't resolve domain: %s\n", uv_err_name(err));
        ret = RC_DOMAIN_RESOLUTION;
      }
      break;
    case AF_INET6:
      if ((err = uv_ip6_name((struct sockaddr_in6 *)hints.ai_addr, ip, INET6_ADDRSTRLEN)) != 0) {
        log_error(logger_id, "Can't resolve domain: %s\n", uv_err_name(err));
        ret = RC_DOMAIN_RESOLUTION;
      }
      break;
    default:
      log_error(logger_id, "Can't resolve domain\n");
      ret = RC_DOMAIN_RESOLUTION;
      break;
  }

  uv_freeaddrinfo(resolver.addrinfo);

  if (ret == RC_OK) {
    log_debug(logger_id, "%s resolved into %s\n", domain, ip);
  }

  return ret;
}

retcode_t tcp_connect(neighbor_t *const neighbor) {
  struct sockaddr_in addr;
  uv_tcp_t *client = NULL;
  uv_connect_t *connection = NULL;
  int ret = 0;

  logger_id = logger_helper_enable(TCP_SERVER_LOGGER_ID, LOGGER_DEBUG, true);

  if ((client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t))) == NULL ||
      (connection = (uv_connect_t *)malloc(sizeof(uv_connect_t))) == NULL) {
    return RC_OOM;
  }

  client->data = neighbor;

  if ((ret = uv_tcp_init(uv_default_loop(), client)) != 0 ||
      (ret = uv_ip4_addr(neighbor->endpoint.ip, neighbor->endpoint.port, &addr)) != 0 ||
      (ret = uv_tcp_nodelay(client, true)) != 0 ||
      (ret = uv_tcp_connect(connection, client, (struct sockaddr *)&addr, tcp_server_on_connect)) != 0) {
    log_warning(logger_id, "Connection to neighbor %s:%d failed: %s\n", neighbor->endpoint.domain,
                neighbor->endpoint.port, uv_err_name(ret));
  }

  return RC_OK;
}
