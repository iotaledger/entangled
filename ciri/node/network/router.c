/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/network/router.h"
#include "utils/logger_helper.h"

#define ROUTER_LOGGER_ID "router"

static UT_icd neighbors_icd = {sizeof(neighbor_t), 0, 0, 0};
static logger_id_t logger_id;

static int neighbor_cmp(void const *const lhs, void const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }

  return endpoint_cmp(&((neighbor_t *)lhs)->endpoint, &((neighbor_t *)rhs)->endpoint) == 0 ? -1 : 0;
}

retcode_t router_init(router_t *const router, iota_node_conf_t *const conf) {
  neighbor_t neighbor;
  char *neighbor_uri, *cpy, *ptr;

  if (router == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(ROUTER_LOGGER_ID, LOGGER_DEBUG, true);

  router->conf = conf;
  utarray_new(router->neighbors, &neighbors_icd);
  rw_lock_handle_init(&router->neighbors_lock);

  if (router->conf->neighbors != NULL) {
    ptr = cpy = strdup(router->conf->neighbors);
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

retcode_t router_destroy(router_t *const router) {
  if (router == NULL) {
    return RC_NULL_PARAM;
  }

  utarray_free(router->neighbors);
  rw_lock_handle_destroy(&router->neighbors_lock);

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t router_neighbor_add(router_t *const router, neighbor_t const *const neighbor) {
  retcode_t ret = RC_OK;
  neighbor_t *elt = NULL;

  if (router == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&router->neighbors_lock);

  elt = utarray_find(router->neighbors, neighbor, neighbor_cmp);

  if (elt != NULL) {
    ret = RC_NEIGHBOR_ALREADY_PAIRED;
  } else {
    utarray_push_back(router->neighbors, neighbor);
    utarray_sort(router->neighbors, neighbor_cmp);
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

  elt = utarray_find(router->neighbors, neighbor, neighbor_cmp);

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
  elt = utarray_find(router->neighbors, &cmp, neighbor_cmp);
  rw_lock_handle_unlock(&router->neighbors_lock);

  return elt;
}
