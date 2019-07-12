/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "uv.h"

#include "ciri/node/network/router.h"
#include "ciri/node/network/tcp_server.h"
#include "ciri/node/node.h"
#include "ciri/node/protocol/type.h"
#include "utils/logger_helper.h"

#define ROUTER_LOGGER_ID "router"

static UT_icd neighbors_icd = {sizeof(neighbor_t), 0, 0, 0};
static logger_id_t logger_id;

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
  }

  return RC_OK;
}

retcode_t router_init(router_t *const router, node_t *const node) {
  retcode_t ret = RC_OK;

  if (router == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(ROUTER_LOGGER_ID, LOGGER_DEBUG, true);

  router->node = node;
  utarray_new(router->neighbors, &neighbors_icd);
  rw_lock_handle_init(&router->neighbors_lock);

  if ((ret = router_neighbors_init(router)) != RC_OK) {
    log_critical(logger_id, "Initializing neighbors failed\n");
    return ret;
  }

  return ret;
}

retcode_t router_destroy(router_t *const router) {
  if (router == NULL) {
    return RC_NULL_PARAM;
  }

  utarray_free(router->neighbors);
  rw_lock_handle_destroy(&router->neighbors_lock);

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t router_neighbor_add(router_t *const router, neighbor_t *const neighbor) {
  retcode_t ret = RC_OK;
  neighbor_t *elt = NULL;

  if (router == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = tcp_server_resolve_domain(neighbor->endpoint.domain, neighbor->endpoint.ip)) != RC_OK) {
    return ret;
  }

  rw_lock_handle_wrlock(&router->neighbors_lock);

  elt = utarray_find(router->neighbors, neighbor, router_neighbor_cmp);

  if (elt != NULL) {
    ret = RC_NEIGHBOR_ALREADY_PAIRED;
  } else {
    utarray_push_back(router->neighbors, neighbor);
    utarray_sort(router->neighbors, router_neighbor_cmp);
  }

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

retcode_t router_neighbor_read_handshake(router_t *const router, char const *const ip, uint16_t const port,
                                         void const *buf, size_t const nread, neighbor_t **const neighbor) {
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

  // Check whether the socket port corresponds to the port advertised in the handshake packet
  handshake_port = ntohs(handshake->port);
  if (port != handshake_port) {
    log_warning(logger_id,
                "Failed handshake with tcp://%s:%d because socket port %d doesn't match advertised port %d\n", ip, port,
                port, handshake_port);
    return RC_FAILED_HANDSHAKE;
  }

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

retcode_t router_neighbors_reconnect_attempt(router_t *const router) {
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;

  rw_lock_handle_wrlock(&router->neighbors_lock);
  NEIGHBORS_FOREACH(router->neighbors, neighbor) {
    if (neighbor->state == NEIGHBOR_DISCONNECTED && neighbor->endpoint.stream == NULL) {
      if ((ret = tcp_server_connect(neighbor)) != RC_OK) {
        log_warning(logger_id, "Trying to reconnect to neighbor %s:%d failed\n", neighbor->endpoint.domain,
                    neighbor->endpoint.port);
      }
    }
  }
  rw_lock_handle_unlock(&router->neighbors_lock);

  return RC_OK;
}
