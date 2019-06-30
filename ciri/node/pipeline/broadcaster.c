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
#define BROADCASTER_TIMEOUT_MS 5000ULL

static logger_id_t logger_id;

/*
 * Private functions
 */

static void *broadcaster_routine(broadcaster_t *const broadcaster) {
  neighbor_t *iter = NULL;
  iota_packet_queue_entry_t *entry = NULL;
  tangle_t tangle;
  lock_handle_t lock_cond;

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

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (broadcaster->running) {
    rw_lock_handle_wrlock(&broadcaster->lock);
    entry = iota_packet_queue_pop(&broadcaster->queue);
    rw_lock_handle_unlock(&broadcaster->lock);

    if (entry == NULL) {
      cond_handle_timedwait(&broadcaster->cond, &lock_cond, BROADCASTER_TIMEOUT_MS);
      continue;
    }

    log_debug(logger_id, "Broadcasting transaction\n");
    rw_lock_handle_rdlock(&broadcaster->node->neighbors_lock);
    LL_FOREACH(broadcaster->node->neighbors, iter) {
      if (!endpoint_cmp(&entry->packet.source, &iter->endpoint)) {
        if (neighbor_send_bytes(broadcaster->node, &tangle, iter, entry->packet.content) != RC_OK) {
          log_warning(logger_id, "Broadcasting transaction failed\n");
        }
      }
    }
    rw_lock_handle_unlock(&broadcaster->node->neighbors_lock);
    free(entry);
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

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

  cond_handle_init(&broadcaster->cond);
  broadcaster->running = false;
  rw_lock_handle_init(&broadcaster->lock);

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
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t broadcaster_stop(broadcaster_t *const broadcaster) {
  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  } else if (broadcaster->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down broadcaster thread\n");
  broadcaster->running = false;
  cond_handle_signal(&broadcaster->cond);
  if (thread_handle_join(broadcaster->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down broadcaster thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t broadcaster_destroy(broadcaster_t *const broadcaster) {
  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  } else if (broadcaster->running) {
    return RC_STILL_RUNNING;
  }

  broadcaster->node = NULL;
  iota_packet_queue_free(&broadcaster->queue);
  rw_lock_handle_destroy(&broadcaster->lock);
  cond_handle_destroy(&broadcaster->cond);
  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t broadcaster_on_next(broadcaster_t *const broadcaster, iota_packet_t const *const packet) {
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
