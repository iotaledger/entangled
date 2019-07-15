/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/network/router.h"
#include "ciri/node/network/tcp_server.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define TCP_SERVER_LOGGER_ID "tcp_server"

static logger_id_t logger_id;
static uv_tcp_t *server = NULL;
static uv_async_t *async = NULL;
static uv_timer_t *reconnect_timer = NULL;

/*
 * Private functions
 */

static void tcp_server_alloc_buffer(uv_handle_t *const handle, size_t suggested_size, uv_buf_t *const buf) {
  UNUSED(handle);

  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

static void tcp_server_on_close(uv_handle_t *const handle) {
  if (handle != NULL) {
    free(handle);
  }
}

static void tcp_server_on_walk(uv_handle_t *const handle, void *arg) {
  UNUSED(arg);

  uv_close(handle, tcp_server_on_close);
}

static void tcp_server_on_async(uv_async_t *const handle) {
  UNUSED(handle);

  uv_stop(uv_default_loop());
}

static void tcp_server_on_write(uv_write_t *const req, int const status) {
  if (status) {
    log_warning(logger_id, "Writing data failed: %s\n", uv_strerror(status));
  }
  free(req->data);
  free(req);
}

static void tcp_server_on_read(uv_stream_t *const client, ssize_t const nread, uv_buf_t const *const buf) {
  neighbor_t *neighbor = (neighbor_t *)client->data;
  router_t *router = &((node_t *)server->data)->router;

  if (nread < 0) {
    if (nread != UV_EOF) {
      log_warning(logger_id, "Read error: %s\n", uv_err_name(nread));
    } else if (neighbor != NULL) {
      log_info(logger_id, "Connection with neighbor tcp://%s:%d lost\n", neighbor->endpoint.domain,
               neighbor->endpoint.port);
      neighbor->state = NEIGHBOR_DISCONNECTED;
      neighbor->endpoint.stream = NULL;
    }
    uv_close((uv_handle_t *)client, tcp_server_on_close);
  } else if (nread > 0) {
    if (neighbor == NULL || neighbor->state == NEIGHBOR_HANDSHAKING) {
      char host[NI_MAXHOST];
      char serv[NI_MAXSERV];
      uint16_t port;
      struct sockaddr_storage addr;
      int len = sizeof(struct sockaddr_storage);

      if (uv_tcp_getpeername((uv_tcp_t *)client, (struct sockaddr *)&addr, &len) != 0 ||
          getnameinfo((struct sockaddr *)&addr, sizeof(addr), host, NI_MAXHOST, serv, NI_MAXSERV,
                      NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
        log_warning(logger_id, "Unable to get peer information\n");
        uv_close((uv_handle_t *)client, tcp_server_on_close);
        return;
      }
      port = atoi(serv);

      log_debug(logger_id, "Initiating handshake with tcp://%s:%d\n", host, port);
      if (router_neighbor_read_handshake(router, host, port, buf->base, nread, &neighbor) == RC_OK) {
        log_info(logger_id, "Connection with neighbor tcp://%s:%d established\n", neighbor->endpoint.domain,
                 neighbor->endpoint.port);
        client->data = neighbor;
        neighbor->endpoint.stream = client;
        neighbor->state = NEIGHBOR_READY_FOR_MESSAGES;
      } else {
        uv_close((uv_handle_t *)client, tcp_server_on_close);
      }
    } else {
      log_debug(logger_id, "Packet received from neighbor tcp://%s:%d\n", neighbor->endpoint.domain,
                neighbor->endpoint.port);
      if (router_neighbor_read(router, neighbor, buf->base, nread) != RC_OK) {
        log_warning(logger_id, "Read error from neighbor tcp://%s:%d\n", neighbor->endpoint.domain,
                    neighbor->endpoint.port);
      }
    }
  }

  if (buf->base) {
    free(buf->base);
  }
}

static void tcp_server_on_new_connection(uv_stream_t *const server, int const status) {
  int ret = 0;
  uv_tcp_t *client = NULL;
  router_t *router = &((node_t *)server->data)->router;

  log_debug(logger_id, "New TCP connection\n");

  if (status < 0) {
    log_warning(logger_id, "New connection failed: %s\n", uv_strerror(status));
    return;
  }

  if ((client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t))) == NULL) {
    log_warning(logger_id, "Allocation of new client failed\n");
    return;
  }
  client->data = NULL;

  if ((ret = uv_tcp_init(uv_default_loop(), client)) != 0 || (ret = uv_tcp_nodelay(client, true)) != 0) {
    log_warning(logger_id, "Initializing new client failed: %s\n", uv_err_name(ret));
    uv_close((uv_handle_t *)client, tcp_server_on_close);
    return;
  }

  if ((ret = uv_accept(server, (uv_stream_t *)client)) == 0) {
    protocol_handshake_t handshake;
    uint16_t handshake_size = 0;

    handshake_init(&handshake, router->node->conf.neighboring_port, router->node->conf.coordinator_address,
                   router->node->conf.mwm, &handshake_size);
    if (tcp_server_write((uv_stream_t *)client, HANDSHAKE, &handshake, handshake_size) != RC_OK) {
      log_error(logger_id, "Sending handshake to new peer failed\n");
      uv_close((uv_handle_t *)client, tcp_server_on_close);
    }
    if ((ret = uv_read_start((uv_stream_t *)client, tcp_server_alloc_buffer, tcp_server_on_read)) != 0) {
      log_error(logger_id, "Starting to read from new client failed: %s\n", uv_err_name(ret));
      uv_close((uv_handle_t *)client, tcp_server_on_close);
    }
  } else {
    log_warning(logger_id, "Accepting new connection failed: %s\n", uv_err_name(ret));
    uv_close((uv_handle_t *)client, tcp_server_on_close);
  }
}

static void tcp_server_on_connect(uv_connect_t *const connection, int const status) {
  int ret = 0;
  uv_stream_t *client = connection->handle;
  neighbor_t *neighbor = (neighbor_t *)client->data;
  router_t *router = &((node_t *)server->data)->router;
  protocol_handshake_t handshake;
  uint16_t handshake_size = 0;

  if (status < 0) {
    log_warning(logger_id, "Connection to neighbor %s:%d failed: %s\n", neighbor->endpoint.domain,
                neighbor->endpoint.port, uv_strerror(status));
    free(connection);
    return;
  }

  neighbor->state = NEIGHBOR_HANDSHAKING;

  handshake_init(&handshake, router->node->conf.neighboring_port, router->node->conf.coordinator_address,
                 router->node->conf.mwm, &handshake_size);
  if (tcp_server_write((uv_stream_t *)client, HANDSHAKE, &handshake, handshake_size) != RC_OK) {
    log_error(logger_id, "Sending handshake to new peer failed\n");
    uv_close((uv_handle_t *)client, tcp_server_on_close);
  }

  if ((ret = uv_read_start(client, tcp_server_alloc_buffer, tcp_server_on_read)) != 0) {
    log_error(logger_id, "Starting to read from %s:%d failed: %s\n", neighbor->endpoint.domain, neighbor->endpoint.port,
              uv_err_name(ret));
    uv_close((uv_handle_t *)client, tcp_server_on_close);
  }

  free(connection);
}

static void tcp_server_on_reconnect_timer(uv_timer_t *const handle) {
  if (router_neighbors_reconnect_attempt(&((node_t *)handle->data)->router) != RC_OK) {
    log_warning(logger_id, "Attempt to reconnect disconnected neighbors failed\n");
  }
}

static void *tcp_server_routine(void *param) {
  UNUSED(param);

  log_info(logger_id, "Starting TCP server on port %d\n", ((node_t *)server->data)->conf.neighboring_port);
  if (uv_run(uv_default_loop(), UV_RUN_DEFAULT) != 0) {
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

  if ((server = (uv_tcp_t *)malloc(sizeof(uv_tcp_t))) == NULL ||
      (async = (uv_async_t *)malloc(sizeof(uv_async_t))) == NULL ||
      (reconnect_timer = (uv_timer_t *)malloc(sizeof(uv_timer_t))) == NULL) {
    return RC_OOM;
  }

  tcp_server->running = false;
  tcp_server->node = node;
  server->data = node;
  reconnect_timer->data = node;

  if ((ret = uv_tcp_init(uv_default_loop(), server)) != 0 ||
      (ret = uv_ip4_addr(node->conf.neighboring_address, node->conf.neighboring_port, &addr)) != 0 ||
      (ret = uv_tcp_nodelay(server, true)) != 0 ||
      (ret = uv_tcp_bind(server, (const struct sockaddr *)&addr, 0)) != 0 ||
      (ret = uv_listen((uv_stream_t *)server, node->conf.max_neighbors, tcp_server_on_new_connection)) != 0 ||
      (ret = uv_async_init(uv_default_loop(), async, tcp_server_on_async)) != 0 ||
      (ret = uv_timer_init(uv_default_loop(), reconnect_timer)) != 0) {
    log_critical(logger_id, "TCP server initialization failed: %s\n", uv_err_name(ret));
    return RC_TCP_SERVER_INIT;
  }

  return RC_OK;
}

retcode_t tcp_server_start(tcp_server_t *const tcp_server) {
  int ret = 0;

  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning TCP server thread\n");
  tcp_server->running = true;
  if (thread_handle_create(&tcp_server->thread, (thread_routine_t)tcp_server_routine, NULL) != 0) {
    log_critical(logger_id, "Spawning TCP server thread failed\n");
    return RC_THREAD_CREATE;
  }
  if ((ret = uv_timer_start(reconnect_timer, tcp_server_on_reconnect_timer, 0,
                            tcp_server->node->conf.reconnect_attempt_interval * 1000)) != 0) {
    log_critical(logger_id, "Starting reconnect attempt timer failed: %s\n", uv_err_name(ret));
  }

  return RC_OK;
}

retcode_t tcp_server_stop(tcp_server_t *const tcp_server) {
  retcode_t ret = RC_OK;
  int err = 0;

  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  } else if (tcp_server->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down TCP server thread\n");
  tcp_server->running = false;

  uv_walk(uv_default_loop(), tcp_server_on_walk, NULL);

  if ((ret = uv_async_send(async)) != 0) {
    log_error(logger_id, "Sending async request to stop event loop failed: %s\n", uv_err_name(err));
  }

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

  if (uv_loop_close(uv_default_loop()) != 0) {
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

retcode_t tcp_server_connect(neighbor_t *const neighbor) {
  struct sockaddr_in addr;
  uv_tcp_t *client = NULL;
  uv_connect_t *connection = NULL;
  int ret = 0;

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

retcode_t tcp_server_write(uv_stream_t *const stream, packet_type_t const type, void *const buffer,
                           uint16_t const buffer_size) {
  retcode_t ret = RC_OK;
  int err = 0;
  uv_write_t *req = NULL;
  protocol_header_t header;

  if (stream == NULL || buffer == NULL) {
    return RC_NULL_PARAM;
  }

  if ((req = malloc(sizeof(uv_write_t))) == NULL) {
    return RC_OOM;
  }

  header.type = type;
  header.length = htons(buffer_size);

  uv_buf_t buf = uv_buf_init(malloc(HEADER_BYTES_LENGTH + buffer_size), HEADER_BYTES_LENGTH + buffer_size);
  memcpy(buf.base, &header, HEADER_BYTES_LENGTH);
  memcpy(buf.base + HEADER_BYTES_LENGTH, buffer, buffer_size);

  req->data = buf.base;

  lock_handle_lock(&((neighbor_t *)stream->data)->buffer_lock);
  if ((err = uv_write(req, stream, &buf, 1, tcp_server_on_write)) != 0) {
    log_warning(logger_id, "Writing failed: %s\n", uv_err_name(err));
    ret = RC_WRITE_TCP_FAILED;
  }
  lock_handle_unlock(&((neighbor_t *)stream->data)->buffer_lock);

  return ret;
}
