/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_LOGGER_HELPER_H_
#define COMMON_LOGGER_HELPER_H_

#include <logger.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct logger_t logger_t;

extern void logger_helper_init(const char* const logger_id,
                               logger_level_t level, bool enable_color);
extern void logger_helper_destroy(const char* const logger_id);
extern void logger_helper_print(const char* const logger_id,
                                logger_level_t level, const char* format, ...);
extern void logger_helper_printer(const char* const string);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_LOGGER_HELPER_H_
