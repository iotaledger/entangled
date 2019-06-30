/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/broadcaster.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"

#define BROADCASTER_LOGGER_ID "broadcaster"

static logger_id_t logger_id;

/*
 * Private functions
 */

static void *broadcaster_routine(broadcaster_t *const broadcaster) {
  tangle_t tangle;
  lock_handle_t cond_lock;
  neighbor_t *neighbor = NULL;
  iota_packet_queue_entry_t *entry = NULL;

  if (broadcaster == NULL) {
    return NULL;
  }

  {
    connection_config_t db_conf = {.db_path = broadcaster->node->conf.tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_init(&cond_lock);
  lock_handle_lock(&cond_lock);

  while (broadcaster->running) {
    rw_lock_handle_wrlock(&broadcaster->lock);
    entry = iota_packet_queue_pop(&broadcaster->queue);
    rw_lock_handle_unlock(&broadcaster->lock);

    if (entry == NULL) {
      cond_handle_wait(&broadcaster->cond, &cond_lock);
      continue;
    }

    log_debug(logger_id, "Broadcasting transaction\n");
    rw_lock_handle_rdlock(&broadcaster->node->neighbors_lock);
    LL_FOREACH(broadcaster->node->neighbors, neighbor) {
      if (!endpoint_cmp(&entry->packet.source, &neighbor->endpoint)) {
        if (neighbor_send_bytes(broadcaster->node, &tangle, neighbor, entry->packet.content) != RC_OK) {
          log_warning(logger_id, "Broadcasting transaction failed\n");
        }
      }
    }
    rw_lock_handle_unlock(&broadcaster->node->neighbors_lock);
    free(entry);
  }

  lock_handle_unlock(&cond_lock);
  lock_handle_destroy(&cond_lock);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t broadcaster_init(broadcaster_t *const broadcaster, node_t *const node) {
  if (broadcaster == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(BROADCASTER_LOGGER_ID, LOGGER_DEBUG, true);

  // Metadata

  if (cond_handle_init(&broadcaster->cond) != 0) {
    log_critical(logger_id, "Initializing condition variable failed\n");
    return RC_COND_INIT;
  }
  if (rw_lock_handle_init(&broadcaster->lock) != 0) {
    log_critical(logger_id, "Initializing lock failed\n");
    return RC_LOCK_INIT;
  }
  broadcaster->running = false;

  // Data

  broadcaster->node = node;
  broadcaster->queue = NULL;

  return RC_OK;
}

retcode_t broadcaster_start(broadcaster_t *const broadcaster) {
  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning broadcaster thread\n");
  broadcaster->running = true;
  if (thread_handle_create(&broadcaster->thread, (thread_routine_t)broadcaster_routine, broadcaster) != 0) {
    log_critical(logger_id, "Spawning broadcaster thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t broadcaster_stop(broadcaster_t *const broadcaster) {
  retcode_t ret = RC_OK;

  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  } else if (broadcaster->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down broadcaster thread\n");
  broadcaster->running = false;
  if (cond_handle_signal(&broadcaster->cond) != 0) {
    log_warning(logger_id, "Signaling condition variable failed\n");
    ret = RC_COND_SIGNAL;
  }
  if (thread_handle_join(broadcaster->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down broadcaster thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t broadcaster_destroy(broadcaster_t *const broadcaster) {
  retcode_t ret = RC_OK;

  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  } else if (broadcaster->running) {
    return RC_STILL_RUNNING;
  }

  // Metadata

  if (cond_handle_destroy(&broadcaster->cond) != 0) {
    log_error(logger_id, "Destroying condition variable failed\n");
    ret = RC_COND_DESTROY;
  }
  if (rw_lock_handle_destroy(&broadcaster->lock) != 0) {
    log_error(logger_id, "Destroying lock failed\n");
    ret = RC_LOCK_DESTROY;
  }

  // Data

  broadcaster->node = NULL;
  iota_packet_queue_free(&broadcaster->queue);

  logger_helper_release(logger_id);

  return ret;
}

retcode_t broadcaster_add(broadcaster_t *const broadcaster, iota_packet_t const *const packet) {
  retcode_t ret = RC_OK;

  if (broadcaster == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&broadcaster->lock);
  ret = iota_packet_queue_push(&broadcaster->queue, packet);
  rw_lock_handle_unlock(&broadcaster->lock);

  if (ret != RC_OK) {
    log_warning(logger_id, "Pushing packet to broadcaster queue failed\n");
    return ret;
  }

  cond_handle_signal(&broadcaster->cond);

  return ret;
}

size_t broadcaster_size(broadcaster_t *const broadcaster) {
  size_t size = 0;

  if (broadcaster == NULL) {
    return 0;
  }

  rw_lock_handle_rdlock(&broadcaster->lock);
  size = iota_packet_queue_count(broadcaster->queue);
  rw_lock_handle_unlock(&broadcaster->lock);

  return size;
}
