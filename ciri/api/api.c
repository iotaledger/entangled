/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/api/api.h"
#include "utils/logger_helper.h"

#define API_LOGGER_ID "api"

static void *iota_api_routine(void *arg) {
  iota_api_t *api = (iota_api_t *)arg;
  while (api->running) {
    sleep(1);
  }
  return NULL;
}

retcode_t iota_api_init(iota_api_t *const api, uint16_t port) {
  if (api == NULL) {
    return RC_CIRI_API_NULL_SELF;
  }

  logger_helper_init(API_LOGGER_ID, LOGGER_DEBUG, true);
  memset(api, 0, sizeof(iota_api_t));
  api->running = false;
  api->port = port;
  return RC_OK;
}

retcode_t iota_api_start(iota_api_t *const api) {
  if (api == NULL) {
    return RC_CIRI_API_NULL_SELF;
  }

  api->running = true;
  log_info(API_LOGGER_ID, "Spawning cIRI API thread\n");
  if (thread_handle_create(&api->thread, (thread_routine_t)iota_api_routine,
                           api) != 0) {
    log_critical(API_LOGGER_ID, "Spawning cIRI API thread failed\n");
    return RC_CIRI_API_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t iota_api_stop(iota_api_t *const api) {
  retcode_t ret = RC_OK;

  if (api == NULL) {
    return RC_CIRI_API_NULL_SELF;
  }

  api->running = false;
  log_info(API_LOGGER_ID, "Shutting down cIRI API thread\n");
  if (thread_handle_join(api->thread, NULL) != 0) {
    log_error(API_LOGGER_ID, "Shutting down cIRI API thread failed\n");
    ret = RC_CIRI_API_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t iota_api_destroy(iota_api_t *const api) {
  if (api == NULL) {
    return RC_CIRI_API_NULL_SELF;
  }
  if (api->running) {
    return RC_CIRI_API_STILL_RUNNING;
  }

  logger_helper_destroy(API_LOGGER_ID);
  return RC_OK;
}
