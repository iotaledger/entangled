/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/logger.h"

#define JSON_LOGGER_ID "json_serializer"

logger_id_t json_logger_id;

void logger_init_json_serializer() {
  json_logger_id = logger_helper_enable(JSON_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(json_logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__, JSON_LOGGER_ID);
}

void logger_destroy_json_serializer() {
  log_info(json_logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__, JSON_LOGGER_ID);
  logger_helper_release(json_logger_id);
}
