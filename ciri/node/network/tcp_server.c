/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include <uv.h>

#include "ciri/node/network/router.h"
#include "ciri/node/network/tcp_server.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define TCP_SERVER_LOGGER_ID "tcp_server"

static logger_id_t logger_id;
static uv_loop_t *loop;
static uv_tcp_t server;

/*
 * Private functions
 */

static void tcp_server_alloc_buffer(uv_handle_t *const handle, size_t suggested_size, uv_buf_t *const buf) {
  UNUSED(handle);

  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

static void tcp_server_on_close(uv_handle_t *const handle) { free(handle); }

static void tcp_server_on_read(uv_stream_t *const client, ssize_t const nread, uv_buf_t const *const buf) {
  neighbor_t *neighbor = (neighbor_t *)client->data;

  log_debug(logger_id, "Packet received from tethered neighbor %s:%d\n", neighbor->endpoint.domain,
            neighbor->endpoint.port);

  if (nread < 0) {
    if (nread != UV_EOF) {
      log_warning(logger_id, "Read error from tethered neighbor %s:%d: %s\n", neighbor->endpoint.domain,
                  neighbor->endpoint.port, uv_err_name(nread));
    } else {
      log_info(logger_id, "Connection with tethered neighbor %s:%d lost\n", neighbor->endpoint.domain,
               neighbor->endpoint.port);
      neighbor->endpoint.stream = NULL;
    }
    uv_close((uv_handle_t *)client, tcp_server_on_close);
  } else if (nread > 0) {
    if (neighbor_read(neighbor, buf->base) != RC_OK) {
      log_warning(logger_id, "Read error from tethered neighbor %s:%d\n", neighbor->endpoint.domain,
                  neighbor->endpoint.port);
    }
  }

  if (buf->base) {
    free(buf->base);
  }
}

void on_new_connection(uv_stream_t *server, int status) {
  fprintf(stderr, "NEW CONN\n");
  if (status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    return;
  }

  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
  } else {
    uv_close((uv_handle_t *)client, NULL);
  }
}

static void *tcp_server_routine(void *param) {
  UNUSED(param);

  log_info(logger_id, "Starting TCP server on port %d\n", ((node_t *)server.data)->conf.neighboring_port);
  if (uv_run(loop, UV_RUN_DEFAULT) != 0) {
    log_critical(logger_id, "Starting TCP server failed\n");
    return NULL;
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t tcp_server_init(tcp_server_t *const tcp_server, node_t *const node) {
  struct sockaddr_in addr;
  int ret = 0;

  if (tcp_server == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(TCP_SERVER_LOGGER_ID, LOGGER_DEBUG, true);

  if ((loop = uv_default_loop()) == NULL) {
    log_critical(logger_id, "Event loop initialization failed\n");
    return RC_EVENT_LOOP;
  }

  tcp_server->running = false;
  server.data = node;

  if ((ret = uv_tcp_init(loop, &server)) != 0 ||
      (ret = uv_ip4_addr(node->conf.neighboring_address, node->conf.neighboring_port, &addr)) != 0 ||
      (ret = uv_tcp_nodelay(&server, true)) != 0 ||
      (ret = uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0)) != 0 ||
      (ret = uv_listen((uv_stream_t *)&server, node->conf.max_neighbors, tcp_server_on_new_connection)) != 0) {
    log_critical(logger_id, "TCP server initialization failed: %s\n", uv_err_name(ret));
    return RC_TCP_SERVER_INIT;
  }

  return RC_OK;
}

retcode_t tcp_server_start(tcp_server_t *const tcp_server) {
  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning TCP server thread\n");
  tcp_server->running = true;
  if (thread_handle_create(&tcp_server->thread, (thread_routine_t)tcp_server_routine, NULL) != 0) {
    log_critical(logger_id, "Spawning TCP server thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t tcp_server_stop(tcp_server_t *const tcp_server) {
  retcode_t ret = RC_OK;

  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  } else if (tcp_server->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down TCP server thread\n");
  tcp_server->running = false;
  uv_stop(loop);
  if (thread_handle_join(tcp_server->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down TCP server thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t tcp_server_destroy(tcp_server_t *const tcp_server) {
  retcode_t ret = RC_OK;

  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  } else if (tcp_server->running) {
    return RC_STILL_RUNNING;
  }

  if (uv_loop_close(loop) != 0) {
    log_error(logger_id, "Closing event loop failed\n");
    ret = RC_EVENT_LOOP;
  }

  logger_helper_release(logger_id);

  return ret;
}

retcode_t tcp_server_resolve_domain(char const *const domain, char *const ip) {
  struct addrinfo hints;
  uv_getaddrinfo_t resolver;
  int err = 0;
  retcode_t ret = RC_OK;

  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  if ((err = uv_getaddrinfo(loop, &resolver, NULL, domain, NULL, &hints)) != 0) {
    log_error(logger_id, "Can't resolve domain: %s\n", uv_err_name(err));
    return RC_DOMAIN_RESOLUTION;
  }

  switch (resolver.addrinfo->ai_family) {
    case AF_INET: {
      if ((err = uv_ip4_name((struct sockaddr_in *)resolver.addrinfo->ai_addr, ip, INET_ADDRSTRLEN)) != 0) {
        log_error(logger_id, "Can't resolve domain: %s\n", uv_err_name(err));
        ret = RC_DOMAIN_RESOLUTION;
      }
      break;
    }
    case AF_INET6: {
      if ((err = uv_ip6_name((struct sockaddr_in6 *)hints.ai_addr, ip, INET6_ADDRSTRLEN)) != 0) {
        log_error(logger_id, "Can't resolve domain: %s\n", uv_err_name(err));
        ret = RC_DOMAIN_RESOLUTION;
      }
      break;
    }
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
