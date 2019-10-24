/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/get_node_api_conf.h"
#include "cclient/api/core/logger.h"

retcode_t iota_client_get_node_api_conf(iota_client_service_t const* const service, get_node_api_conf_res_t* res) {
  log_debug(client_core_logger_id, "[%s:%d]\n", __func__, __LINE__);
  retcode_t result = RC_ERROR;

  if (!service || !res) {
    log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_OOM);
    result = RC_OOM;
    goto done;
  }

  if ((result = service->serializer.vtable.get_node_api_conf_serialize_request(req_buff)) != RC_OK) {
    log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(result));
    goto done;
  }

  if ((result = iota_service_query(service, req_buff, res_buff)) != RC_OK) {
    log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_node_api_conf_deserialize_response(res_buff->data, res);

done:
  char_buffer_free(req_buff);
  char_buffer_free(res_buff);
  return result;
}
