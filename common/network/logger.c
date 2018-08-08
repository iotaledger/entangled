/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdarg.h>

#include "logger.h"
#include "utils/handles/lock.h"

static struct logger_s {
  log_level_t level;
  bool silent;
  FILE *file;
  lock_handle_t lock;
  const char *level_names[];
} logger_g = {LOG_INFO,
              false,
              NULL,
              {},
              {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"}};

void logger_init(log_level_t level, bool silent, FILE *file) {
  logger_g.level = level;
  logger_g.silent = silent;
  logger_g.file = file;
  lock_handle_init(&logger_g.lock);
}

void logger_set_level(log_level_t level) { logger_g.level = level; }

void logger_set_silent(bool silent) { logger_g.silent = silent; }

void logger_log(log_level_t level, char const *fmt, ...) {
  time_t now;
  char buf[32];
  size_t size;
  va_list args;

  if (level < logger_g.level || logger_g.silent || logger_g.file == NULL) {
    return;
  }

  now = time(NULL);
  size = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  buf[size] = '\0';

  lock_handle_lock(&logger_g.lock);
  fprintf(logger_g.file, "[%s] [%-5s] ", buf, logger_g.level_names[level]);
  va_start(args, fmt);
  vfprintf(logger_g.file, fmt, args);
  va_end(args);
  fprintf(logger_g.file, "\n");
  lock_handle_unlock(&logger_g.lock);
}
