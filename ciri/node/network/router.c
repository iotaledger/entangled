/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "uv.h"

#include "ciri/node/network/router.h"
#include "ciri/node/node.h"
#include "ciri/node/protocol/type.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define ROUTER_LOGGER_ID "router"

static UT_icd neighbors_icd = {sizeof(neighbor_t), 0, 0, 0};
static logger_id_t logger_id;
static uv_tcp_t *server = NULL;
static uv_async_t *async = NULL;
static uv_timer_t *reconnect_timer = NULL;

static int router_neighbor_cmp(void const *const lhs, void const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }

  return endpoint_cmp(&((neighbor_t *)lhs)->endpoint, &((neighbor_t *)rhs)->endpoint);
}

static retcode_t router_neighbors_init(router_t *const router) {
  neighbor_t neighbor;
  char *neighbor_uri, *cpy, *ptr;

  if (router->node->conf.neighbors != NULL) {
    if ((ptr = cpy = strdup(router->node->conf.neighbors)) == NULL) {
      return RC_OOM;
    }
    while ((neighbor_uri = strsep(&cpy, " ")) != NULL) {
      if (neighbor_init_with_uri(&neighbor, neighbor_uri) != RC_OK) {
        log_warning(logger_id, "Initializing neighbor with URI %s failed\n", neighbor_uri);
        continue;
      }
      log_info(logger_id, "Adding neighbor %s\n", neighbor_uri);
      if (router_neighbor_add(router, &neighbor) != RC_OK) {
        log_warning(logger_id, "Adding neighbor %s failed\n", neighbor_uri);
      }
    }
    free(ptr);
  } else {
    log_warning(logger_id, "Running node with no neighbors\n");
  }

  return RC_OK;
}

/*
 * Private functions
 */

static void router_alloc_buffer(uv_handle_t *const handle, size_t suggested_size, uv_buf_t *const buf) {
  UNUSED(handle);

  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

static void router_on_close(uv_handle_t *const handle) {
  if (handle != NULL) {
    free(handle);
  }
}

static void router_on_walk(uv_handle_t *const handle, void *arg) {
  UNUSED(arg);

  uv_close(handle, router_on_close);
}

static void router_on_async(uv_async_t *const handle) {
  UNUSED(handle);

  uv_stop(uv_default_loop());
}

static void router_on_write(uv_write_t *const req, int const status) {
  if (status) {
    log_warning(logger_id, "Writing data failed: %s\n", uv_strerror(status));
  }
  free(req->data);
  free(req);
}

static void router_on_async_write(uv_async_t *const handle) {
  neighbor_t *neighbor = (neighbor_t *)handle->data;
  uv_buf_t_queue_entry_t *entry = NULL;
  uv_write_t *req = NULL;
  int err = 0;

  if ((req = malloc(sizeof(uv_write_t))) == NULL) {
    log_warning(logger_id, "Allocating write request failed\n");
    return;
  }

  rw_lock_handle_wrlock(&neighbor->write_queue_lock);
  entry = neighbor_write_queue_pop(neighbor);
  rw_lock_handle_unlock(&neighbor->write_queue_lock);

  if (entry != NULL) {
    req->data = entry->buf.base;
    if ((err = uv_write(req, neighbor->endpoint.stream, &entry->buf, 1, router_on_write)) != 0) {
      log_warning(logger_id, "Writing failed: %s\n", uv_err_name(err));
    } else {
      neighbor->nbr_sent_txs++;
    }
    free(entry);
  }
}

static void router_on_read(uv_stream_t *const client, ssize_t const nread, uv_buf_t const *const buf) {
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
    uv_close((uv_handle_t *)client, router_on_close);
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
        uv_close((uv_handle_t *)client, router_on_close);
        return;
      }
      port = atoi(serv);

      log_debug(logger_id, "Initiating handshake with tcp://%s:%d\n", host, port);
      if (router_read_handshake(router, host, port, buf->base, nread, &neighbor) == RC_OK) {
        log_info(logger_id, "Connection with neighbor tcp://%s:%d established\n", neighbor->endpoint.domain,
                 neighbor->endpoint.port);
        client->data = neighbor;
        neighbor->endpoint.stream = client;
        neighbor->state = NEIGHBOR_READY_FOR_MESSAGES;
      } else {
        uv_close((uv_handle_t *)client, router_on_close);
      }
    } else {
      log_debug(logger_id, "Packet received from neighbor tcp://%s:%d\n", neighbor->endpoint.domain,
                neighbor->endpoint.port);
      if (router_read(router, neighbor, buf->base, nread) != RC_OK) {
        log_warning(logger_id, "Read error from neighbor tcp://%s:%d\n", neighbor->endpoint.domain,
                    neighbor->endpoint.port);
      }
    }
  }

  if (buf->base) {
    free(buf->base);
  }
}

static void router_on_new_connection(uv_stream_t *const server, int const status) {
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
    uv_close((uv_handle_t *)client, router_on_close);
    return;
  }

  if ((ret = uv_accept(server, (uv_stream_t *)client)) == 0) {
    protocol_handshake_t handshake;
    uint16_t handshake_size = 0;

    handshake_init(&handshake, router->node->conf.neighboring_port, router->node->conf.coordinator_address,
                   router->node->conf.mwm, &handshake_size);
    if (router_write((uv_stream_t *)client, HANDSHAKE, &handshake, handshake_size) != RC_OK) {
      log_error(logger_id, "Sending handshake to new peer failed\n");
      uv_close((uv_handle_t *)client, router_on_close);
    }
    if ((ret = uv_read_start((uv_stream_t *)client, router_alloc_buffer, router_on_read)) != 0) {
      log_error(logger_id, "Starting to read from new client failed: %s\n", uv_err_name(ret));
      uv_close((uv_handle_t *)client, router_on_close);
    }
  } else {
    log_warning(logger_id, "Accepting new connection failed: %s\n", uv_err_name(ret));
    uv_close((uv_handle_t *)client, router_on_close);
  }
}

void router_on_connect(uv_connect_t *const connection, int const status) {
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
  if (router_write((uv_stream_t *)client, HANDSHAKE, &handshake, handshake_size) != RC_OK) {
    log_error(logger_id, "Sending handshake to new peer failed\n");
    uv_close((uv_handle_t *)client, router_on_close);
  }

  if ((ret = uv_read_start(client, router_alloc_buffer, router_on_read)) != 0) {
    log_error(logger_id, "Starting to read from %s:%d failed: %s\n", neighbor->endpoint.domain, neighbor->endpoint.port,
              uv_err_name(ret));
    uv_close((uv_handle_t *)client, router_on_close);
  }

  free(connection);
}

static void router_on_reconnect_timer(uv_timer_t *const handle) {
  if (router_reconnect_attempt(&((node_t *)handle->data)->router) != RC_OK) {
    log_warning(logger_id, "Attempt to reconnect disconnected neighbors failed\n");
  }
}

static void *router_routine(void *param) {
  UNUSED(param);

  log_info(logger_id, "Starting TCP server on port %d\n", ((node_t *)server->data)->conf.neighboring_port);
  if (uv_run(uv_default_loop(), UV_RUN_DEFAULT) != 0) {
    log_critical(logger_id, "Starting TCP server failed\n");
    return NULL;
  }

  return NULL;
}

retcode_t router_init(router_t *const router, node_t *const node) {
  retcode_t ret = RC_OK;
  struct sockaddr_in addr;
  int err = 0;

  if (router == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(ROUTER_LOGGER_ID, LOGGER_DEBUG, true);

  if ((server = (uv_tcp_t *)malloc(sizeof(uv_tcp_t))) == NULL ||
      (async = (uv_async_t *)malloc(sizeof(uv_async_t))) == NULL ||
      (reconnect_timer = (uv_timer_t *)malloc(sizeof(uv_timer_t))) == NULL) {
    return RC_OOM;
  }

  router->node = node;
  router->running = false;
  utarray_new(router->neighbors, &neighbors_icd);
  rw_lock_handle_init(&router->neighbors_lock);
  server->data = node;
  reconnect_timer->data = node;

  if ((ret = router_neighbors_init(router)) != RC_OK) {
    log_critical(logger_id, "Initializing neighbors failed\n");
    return ret;
  }

  if ((err = uv_tcp_init(uv_default_loop(), server)) != 0 ||
      (err = uv_ip4_addr(node->conf.neighboring_address, node->conf.neighboring_port, &addr)) != 0 ||
      (err = uv_tcp_nodelay(server, true)) != 0 ||
      (err = uv_tcp_bind(server, (const struct sockaddr *)&addr, 0)) != 0 ||
      (err = uv_listen((uv_stream_t *)server, node->conf.max_neighbors, router_on_new_connection)) != 0 ||
      (err = uv_async_init(uv_default_loop(), async, router_on_async)) != 0 ||
      (err = uv_timer_init(uv_default_loop(), reconnect_timer)) != 0) {
    log_critical(logger_id, "TCP server initialization failed: %s\n", uv_err_name(err));
    return RC_TCP_SERVER_INIT;
  }

  return ret;
}

retcode_t router_start(router_t *const router) {
  int ret = 0;

  if (router == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning TCP server thread\n");
  router->running = true;
  if (thread_handle_create(&router->thread, (thread_routine_t)router_routine, NULL) != 0) {
    log_critical(logger_id, "Spawning TCP server thread failed\n");
    return RC_THREAD_CREATE;
  }
  if ((ret = uv_timer_start(reconnect_timer, router_on_reconnect_timer, 0,
                            router->node->conf.reconnect_attempt_interval * 1000)) != 0) {
    log_critical(logger_id, "Starting reconnect attempt timer failed: %s\n", uv_err_name(ret));
  }

  return RC_OK;
}

retcode_t router_stop(router_t *const router) {
  retcode_t ret = RC_OK;
  int err = 0;

  if (router == NULL) {
    return RC_NULL_PARAM;
  } else if (router->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down TCP server thread\n");
  router->running = false;

  uv_walk(uv_default_loop(), router_on_walk, NULL);

  if ((ret = uv_async_send(async)) != 0) {
    log_error(logger_id, "Sending async request to stop event loop failed: %s\n", uv_err_name(err));
  }

  if (thread_handle_join(router->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down TCP server thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t router_destroy(router_t *const router) {
  int err = 0;
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;

  if (router == NULL) {
    return RC_NULL_PARAM;
  } else if (router->running) {
    return RC_STILL_RUNNING;
  }

  if ((err = uv_loop_close(uv_default_loop())) != 0) {
    log_error(logger_id, "Closing event loop failed: %s\n", uv_err_name(err));
    ret = RC_EVENT_LOOP;
  }

  NEIGHBORS_FOREACH(router->neighbors, neighbor) { neighbor_destroy(neighbor); }
  utarray_free(router->neighbors);
  rw_lock_handle_destroy(&router->neighbors_lock);

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t router_neighbor_add(router_t *const router, neighbor_t *const neighbor) {
  retcode_t ret = RC_OK;
  neighbor_t *elt = NULL;
  int err = 0;

  if (router == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = router_resolve(neighbor->endpoint.domain, neighbor->endpoint.ip)) != RC_OK) {
    return ret;
  }

  rw_lock_handle_wrlock(&router->neighbors_lock);

  elt = utarray_find(router->neighbors, neighbor, router_neighbor_cmp);

  if (elt != NULL) {
    ret = RC_NEIGHBOR_ALREADY_PAIRED;
  } else {
    utarray_push_back(router->neighbors, neighbor);
    elt = (neighbor_t *)utarray_back(router->neighbors);
    rw_lock_handle_init(&elt->write_queue_lock);
    if ((elt->writer = (uv_async_t *)malloc(sizeof(uv_async_t))) == NULL) {
      ret = RC_OOM;
      goto done;
    }
    if ((err = uv_async_init(uv_default_loop(), elt->writer, router_on_async_write)) != 0) {
      log_warning(logger_id, "Initializing async writer failed: %s\n", uv_err_name(err));
      ret = RC_ASYNC_INIT_FAILED;
      goto done;
    }
    elt->writer->data = elt;
    utarray_sort(router->neighbors, router_neighbor_cmp);
  }

done:

  rw_lock_handle_unlock(&router->neighbors_lock);

  return ret;
}

retcode_t router_neighbor_remove(router_t *const router, neighbor_t const *const neighbor) {
  retcode_t ret = RC_OK;
  neighbor_t *elt = NULL;

  if (router == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&router->neighbors_lock);

  elt = utarray_find(router->neighbors, neighbor, router_neighbor_cmp);

  if (elt == NULL) {
    ret = RC_NEIGHBOR_NOT_PAIRED;
  } else {
    utarray_erase(router->neighbors, utarray_eltidx(router->neighbors, elt), 1);
  }

  rw_lock_handle_unlock(&router->neighbors_lock);

  return ret;
}

size_t router_neighbors_count(router_t *const router) {
  size_t count = 0;

  if (router == NULL) {
    return 0;
  }

  rw_lock_handle_rdlock(&router->neighbors_lock);
  count = router->neighbors ? utarray_len(router->neighbors) : 0;
  rw_lock_handle_unlock(&router->neighbors_lock);

  return count;
}

neighbor_t *router_neighbor_find_by_endpoint(router_t *const router, endpoint_t const *const endpoint) {
  if (router == NULL || endpoint == NULL) {
    return NULL;
  }

  return router_neighbor_find_by_endpoint_values(router, endpoint->ip, endpoint->port);
}

neighbor_t *router_neighbor_find_by_endpoint_values(router_t *const router, char const *const ip, uint16_t const port) {
  neighbor_t cmp;
  neighbor_t *elt;

  if (router == NULL) {
    return NULL;
  }

  if (neighbor_init_with_values(&cmp, ip, port)) {
    return NULL;
  }

  rw_lock_handle_rdlock(&router->neighbors_lock);
  elt = utarray_find(router->neighbors, &cmp, router_neighbor_cmp);
  rw_lock_handle_unlock(&router->neighbors_lock);

  return elt;
}

retcode_t router_read_handshake(router_t *const router, char const *const ip, uint16_t const port, void const *buf,
                                size_t const nread, neighbor_t **const neighbor) {
  protocol_header_t const *header = NULL;
  protocol_handshake_t const *handshake = NULL;
  int protocol_version = 0;
  uint16_t header_length = 0;
  uint16_t handshake_port = 0;

  if (router == NULL || ip == NULL || buf == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  *neighbor = NULL;

  // Check whether we have a complete header
  if (nread < HEADER_BYTES_LENGTH) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because invalid header size %d\n", ip, port, nread);
    return RC_INVALID_PACKET;
  }
  header = (protocol_header_t const *)buf;

  // Check if the packet is really a handshake
  if (header->type != HANDSHAKE) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because packet is not a handshake\n", ip, port);
    return RC_INVALID_PACKET_TYPE;
  }

  // Check whether we have a complete handshake packet
  header_length = ntohs(header->length);
  if (nread != HEADER_BYTES_LENGTH + header_length || header_length < HANDSHAKE_MIN_BYTES_LENGTH ||
      header_length > HANDSHAKE_MAX_BYTES_LENGTH) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because invalid packet size %d\n", ip, port,
                header_length);
    return RC_INVALID_PACKET;
  }
  handshake = (protocol_handshake_t const *)(buf + HEADER_BYTES_LENGTH);

  // Check whether the same coordinator address is used
  if (memcmp(router->node->conf.coordinator_address, handshake->coordinator_address,
             HANDSHAKE_COORDINATOR_ADDRESS_BYTES_LENGTH) != 0) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because different coordinator addresses are used\n", ip,
                port);
    return RC_FAILED_HANDSHAKE;
  }

  // Check whether the same MWM is used
  if (router->node->conf.mwm != handshake->mwm) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because different MWM values are used: %d and %d\n", ip,
                port, router->node->conf.mwm, handshake->mwm);
    return RC_FAILED_HANDSHAKE;
  }

  // Check whether we support the supported protocol versions by the neighbor
  if ((protocol_version = handshake_supported_version(
           handshake_supported_protocol_versions, sizeof(handshake_supported_protocol_versions),
           handshake->supported_versions, header_length - HANDSHAKE_MIN_BYTES_LENGTH + 1)) < 0) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because of protocol version %d mismatch\n", ip, port,
                protocol_version);
    return RC_FAILED_HANDSHAKE;
  }

  // Check whether the peer is an allowed neighbor
  handshake_port = ntohs(handshake->port);
  if ((*neighbor = router_neighbor_find_by_endpoint_values(router, ip, handshake_port)) == NULL) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because the peer is a non-tethered neighbor\n", ip, port);
    return RC_FAILED_HANDSHAKE;
  }
  // TODO auto-peering

  // Check if the neighbor is not already connected
  if ((*neighbor)->endpoint.stream != NULL) {
    log_warning(logger_id, "Failed handshake with tcp://%s:%d because it's an already connected neighbor\n", ip, port);
    return RC_FAILED_HANDSHAKE;
  }

  (*neighbor)->protocol_version = protocol_version;

  return RC_OK;
}

retcode_t router_read(router_t *const router, neighbor_t *const neighbor, void const *const buffer,
                      size_t const buffer_size) {
  retcode_t ret = RC_OK;
  protocol_header_t const *header = NULL;
  uint16_t header_length = 0;
  size_t chunk_size = 0;

  if (router == NULL || neighbor == NULL || buffer == NULL) {
    return RC_NULL_PARAM;
  }

  if (buffer_size == 0) {
    return RC_OK;
  }

  // We concatenate read data to previously read data
  chunk_size = MIN(PACKET_MAX_BYTES_LENGTH - neighbor->buffer_size, buffer_size);
  memcpy(neighbor->buffer + neighbor->buffer_size, buffer, chunk_size);
  neighbor->buffer_size += chunk_size;

  // We haven't received a full header yet
  if (neighbor->buffer_size <= HEADER_BYTES_LENGTH) {
    return RC_OK;
  }

  header = (protocol_header_t const *)neighbor->buffer;
  header_length = ntohs(header->length);

  // We haven't received the full packet yet
  if (neighbor->buffer_size - HEADER_BYTES_LENGTH < header_length) {
    return RC_OK;
  }

  if (header->type < PACKET_TYPES_NUM) {
    switch (header->type) {
      case HANDSHAKE:
        break;
      case GOSSIP: {
        protocol_gossip_t gossip;
        void const *ptr = neighbor->buffer + HEADER_BYTES_LENGTH;
        size_t offset = 0;
        size_t variable_size = 0;

        // Check whether the packet length matches gossip packet length
        if (header_length < GOSSIP_MIN_BYTES_LENGTH || header_length > GOSSIP_MAX_BYTES_LENGTH) {
          log_warning(logger_id, "Invalid packet size %d from neighbor tcp://%s:%d\n", header_length,
                      neighbor->endpoint.domain, neighbor->endpoint.port);
          return RC_INVALID_PACKET;
        }

        memset(&gossip, 0, sizeof(protocol_gossip_t));

        variable_size = header_length - GOSSIP_NON_SIG_BYTES_LENGTH - GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH;
        memcpy(gossip.content + offset, ptr, variable_size);
        ptr += variable_size;
        offset += variable_size;

        memset(gossip.content + offset, 0, GOSSIP_SIG_MAX_BYTES_LENGTH - variable_size);
        offset += GOSSIP_SIG_MAX_BYTES_LENGTH - variable_size;

        memcpy(gossip.content + offset, ptr, GOSSIP_NON_SIG_BYTES_LENGTH);
        ptr += GOSSIP_NON_SIG_BYTES_LENGTH;
        offset += GOSSIP_NON_SIG_BYTES_LENGTH;

        memcpy(gossip.content + offset, ptr, GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH);
        ptr += GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH;
        offset += GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH;

        protocol_gossip_set_endpoint(&gossip, neighbor->endpoint.ip, neighbor->endpoint.port);

        if ((ret = processor_stage_add(&router->node->processor, &gossip)) != RC_OK) {
          log_warning(logger_id, "Pushing gossip packet from tcp://%s:%d failed\n", neighbor->endpoint.domain,
                      neighbor->endpoint.port);
        }
        break;
      }
      default:
        log_warning(logger_id, "Invalid packet type %d from neighbor tcp://%s:%d\n", header->type,
                    neighbor->endpoint.domain, neighbor->endpoint.port);
        return RC_INVALID_PACKET_TYPE;
        break;
    }
  } else {
    log_warning(logger_id, "Invalid packet type %d from neighbor tcp://%s:%d\n", header->type,
                neighbor->endpoint.domain, neighbor->endpoint.port);
    return RC_INVALID_PACKET_TYPE;
  }

  memmove(neighbor->buffer, neighbor->buffer + HEADER_BYTES_LENGTH + header_length,
          PACKET_MAX_BYTES_LENGTH - (HEADER_BYTES_LENGTH + header_length));
  neighbor->buffer_size -= (HEADER_BYTES_LENGTH + header_length);

  return router_read(router, neighbor, buffer + chunk_size, buffer_size - chunk_size);
}

retcode_t router_reconnect_attempt(router_t *const router) {
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;

  rw_lock_handle_wrlock(&router->neighbors_lock);
  NEIGHBORS_FOREACH(router->neighbors, neighbor) {
    if (neighbor->state == NEIGHBOR_DISCONNECTED && neighbor->endpoint.stream == NULL) {
      if ((ret = router_connect(neighbor)) != RC_OK) {
        log_warning(logger_id, "Trying to reconnect to neighbor %s:%d failed\n", neighbor->endpoint.domain,
                    neighbor->endpoint.port);
      }
    }
  }
  rw_lock_handle_unlock(&router->neighbors_lock);

  return RC_OK;
}

retcode_t router_write(uv_stream_t *const stream, packet_type_t const type, void *const buffer,
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

  if ((err = uv_write(req, stream, &buf, 1, router_on_write)) != 0) {
    log_warning(logger_id, "Writing failed: %s\n", uv_err_name(err));
    ret = RC_WRITE_TCP_FAILED;
  }

  return ret;
}

retcode_t router_resolve(char const *const domain, char *const ip) {
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

retcode_t router_connect(neighbor_t *const neighbor) {
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
      (ret = uv_tcp_connect(connection, client, (struct sockaddr *)&addr, router_on_connect)) != 0) {
    log_warning(logger_id, "Connection to neighbor %s:%d failed: %s\n", neighbor->endpoint.domain,
                neighbor->endpoint.port, uv_err_name(ret));
  }

  return RC_OK;
}
