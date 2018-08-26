/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/logger_helper.h"
#include "utils/handles/lock.h"

static lock_handle_t lock;

void logger_helper_init(const char* const logger_id, logger_level_t level,
                        bool enable_color) {
  logger_id_t id;
  lock_handle_init(&lock);
  /* get a logging id, enable it and set log level */
  id = logger_id_request(logger_id);
  logger_id_enable(id);
  logger_id_level_set(id, level);
  logger_id_prefix_set(id,
                       (LOGGER_PFX_DATE | LOGGER_PFX_NAME | LOGGER_PFX_LEVEL));
  if (enable_color) {
    logger_color_prefix_enable();
    logger_color_message_enable();
    logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK,
                                LOGGER_ATTR_BRIGHT | LOGGER_ATTR_UNDERLINE);
  }
}

void logger_helper_destroy(const char* const logger_id) {
  logger_id_t id;
  lock_handle_lock(&lock);
  id = logger_id_request(logger_id);
  logger_id_release(id);
  lock_handle_unlock(&lock);
}

void logger_helper_print(const char* const logger_id, logger_level_t level,
                         const char* format, ...) {
  logger_id_t id;
  va_list argp;

  va_start(argp, format);
  lock_handle_lock(&lock);
  id = logger_id_request(logger_id);
  logger_va(id, level, format, argp);
  lock_handle_unlock(&lock);
  va_end(argp);
}
