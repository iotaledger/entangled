/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_LOGGER_HELPER_H_
#define COMMON_LOGGER_HELPER_H_

#include <stdbool.h>

#include "logger.h"

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct logger_t logger_t;

#define log_debug(id, ...) logger_helper_print(id, LOGGER_DEBUG, __VA_ARGS__)
#define log_info(id, ...) logger_helper_print(id, LOGGER_INFO, __VA_ARGS__)
#define log_notice(id, ...) logger_helper_print(id, LOGGER_NOTICE, __VA_ARGS__)
#define log_warning(id, ...) logger_helper_print(id, LOGGER_WARNING, __VA_ARGS__)
#define log_error(id, ...) logger_helper_print(id, LOGGER_ERR, __VA_ARGS__)
#define log_critical(id, ...) logger_helper_print(id, LOGGER_CRIT, __VA_ARGS__)
#define log_alert(id, ...) logger_helper_print(id, LOGGER_ALERT, __VA_ARGS__)
#define log_emergency(id, ...) logger_helper_print(id, LOGGER_EMERG, __VA_ARGS__)

retcode_t logger_helper_init(logger_level_t level);
retcode_t logger_helper_destroy();
logger_id_t logger_helper_enable(char const* const logger_name, logger_level_t const level, bool const enable_color);
void logger_helper_release(logger_id_t const logger_id);
void logger_helper_print(logger_id_t const logger_id, logger_level_t const level, char const* const format, ...);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_LOGGER_HELPER_H_
