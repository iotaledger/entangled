/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_LOGGER_H__
#define __COMMON_NETWORK_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>

typedef enum log_level_e {
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_FATAL
} log_level_t;

#define log_trace(...) logger_log(LOG_TRACE, __VA_ARGS__)
#define log_debug(...) logger_log(LOG_DEBUG, __VA_ARGS__)
#define log_info(...) logger_log(LOG_INFO, __VA_ARGS__)
#define log_warn(...) logger_log(LOG_WARN, __VA_ARGS__)
#define log_error(...) logger_log(LOG_ERROR, __VA_ARGS__)
#define log_fatal(...) logger_log(LOG_FATAL, __VA_ARGS__)

void logger_init(log_level_t level, bool silent, FILE *file);
void logger_set_level(log_level_t level);
void logger_set_silent(bool silent);

void logger_log(log_level_t level, char const *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_LOGGER_H__
