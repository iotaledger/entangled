/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/logger.h"

#define CCLIENT_CORE_LOGGER_ID "cclient_core"

logger_id_t client_core_logger_id;

void logger_init_client_core() {
  client_core_logger_id = logger_helper_enable(CCLIENT_CORE_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(client_core_logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__, CCLIENT_CORE_LOGGER_ID);
}

void logger_destroy_client_core() {
  log_info(client_core_logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__, CCLIENT_CORE_LOGGER_ID);
  logger_helper_release(client_core_logger_id);
}
