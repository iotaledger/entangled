/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/check_consistency.h"
#include "cclient/api/core/logger.h"

retcode_t iota_client_check_consistency(const iota_client_service_t* const service, check_consistency_req_t* const req,
                                        check_consistency_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(client_core_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.check_consistency_serialize_request(&service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.check_consistency_deserialize_response(&service->serializer, res_buff->data, res);

done:
  char_buffer_free(req_buff);
  char_buffer_free(res_buff);
  return result;
}
