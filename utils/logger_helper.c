/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/logger_helper.h"
#include "utils/handles/lock.h"

static lock_handle_t lock;

retcode_t logger_helper_init() {
  if (LOGGER_VERSION != logger_version()) {
    return RC_UTILS_INVALID_LOGGER_VERSION;
  }

  logger_init();
  logger_color_prefix_enable();
  logger_color_message_enable();
  logger_output_register(stdout);
  logger_output_level_set(stdout, LOGGER_WARNING);

  lock_handle_init(&lock);

  return RC_OK;
}

retcode_t logger_helper_destroy() {
  lock_handle_destroy(&lock);

  return RC_OK;
}

void logger_helper_enable(char const* const logger_id,
                          logger_level_t const level, bool const enable_color) {
  logger_id_t id;

  lock_handle_lock(&lock);
  id = logger_id_request(logger_id);
  logger_id_enable(id);
  logger_id_level_set(id, level);
  logger_id_prefix_set(id,
                       (LOGGER_PFX_DATE | LOGGER_PFX_NAME | LOGGER_PFX_LEVEL));
  if (enable_color) {
    logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK,
                                LOGGER_ATTR_BRIGHT | LOGGER_ATTR_UNDERLINE);
  }
  lock_handle_unlock(&lock);
}

void logger_helper_release(char const* const logger_id) {
  logger_id_t id;

  lock_handle_lock(&lock);
  id = logger_id_request(logger_id);
  logger_id_release(id);
  lock_handle_unlock(&lock);
}

void logger_helper_print(char const* const logger_id,
                         logger_level_t const level, char const* const format,
                         ...) {
  logger_id_t id;
  va_list argp;

  va_start(argp, format);
  lock_handle_lock(&lock);
  id = logger_id_request(logger_id);
  logger_va(id, level, format, argp);
  lock_handle_unlock(&lock);
  va_end(argp);
}
