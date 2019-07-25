/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/validator.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"

#define VALIDATOR_LOGGER_ID "validator"

static logger_id_t logger_id;

/*
 * Private functions
 */

static void *validator_stage_routine(validator_stage_t *const validator) {
  lock_handle_t lock_cond;

  if (validator == NULL) {
    return NULL;
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (validator->running) {
    lock_handle_lock(&validator->lock);
    // entry = protocol_gossip_queue_pop(&validator->queue);
    lock_handle_unlock(&validator->lock);

    // if (entry == NULL) {
    //   cond_handle_wait(&validator->cond, &lock_cond);
    //   continue;
    // }

    // free(entry);
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  return NULL;
}

/*
 * Public functions
 */

retcode_t validator_stage_init(validator_stage_t *const validator, node_t *const node) {
  if (validator == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);

  validator->running = false;
  // validator->queue = NULL;
  lock_handle_init(&validator->lock);
  cond_handle_init(&validator->cond);
  validator->node = node;

  return RC_OK;
}

retcode_t validator_stage_start(validator_stage_t *const validator) {
  if (validator == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning validator stage thread\n");
  validator->running = true;
  if (thread_handle_create(&validator->thread, (thread_routine_t)validator_stage_routine, validator) != 0) {
    log_critical(logger_id, "Spawning validator stage thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t validator_stage_stop(validator_stage_t *const validator) {
  if (validator == NULL) {
    return RC_NULL_PARAM;
  } else if (validator->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down validator stage thread\n");
  validator->running = false;
  cond_handle_signal(&validator->cond);
  if (thread_handle_join(validator->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down validator stage thread failed\n");
    return RC_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t validator_stage_destroy(validator_stage_t *const validator) {
  if (validator == NULL) {
    return RC_NULL_PARAM;
  } else if (validator->running) {
    return RC_STILL_RUNNING;
  }

  // protocol_gossip_queue_free(&validator->queue);
  lock_handle_destroy(&validator->lock);
  cond_handle_destroy(&validator->cond);
  validator->node = NULL;

  logger_helper_release(logger_id);

  return RC_OK;
}

// retcode_t validator_stage_add(validator_stage_t *const validator, protocol_gossip_t const *const packet) {
//   retcode_t ret = RC_OK;
//
//   if (validator == NULL) {
//     return RC_NULL_PARAM;
//   }
//
//   lock_handle_lock(&validator->lock);
//   ret = protocol_gossip_queue_push(&validator->queue, packet);
//   lock_handle_unlock(&validator->lock);
//
//   if (ret != RC_OK) {
//     log_warning(logger_id, "Pushing packet to validator stage queue failed\n");
//     return ret;
//   } else {
//     cond_handle_signal(&validator->cond);
//   }
//
//   return RC_OK;
// }

size_t validator_stage_size(validator_stage_t *const validator) {
  size_t size = 0;

  if (validator == NULL) {
    return 0;
  }

  lock_handle_lock(&validator->lock);
  // size = protocol_gossip_queue_count(validator->queue);
  lock_handle_unlock(&validator->lock);

  return size;
}
