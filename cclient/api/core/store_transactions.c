/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/store_transactions.h"
#include "cclient/api/core/logger.h"

retcode_t iota_client_store_transactions(iota_client_service_t const* const service,
                                         store_transactions_req_t const* const req) {
  retcode_t result = RC_ERROR;
  log_debug(client_core_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!service || !req) {
    log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_OOM);
    result = RC_OOM;
    goto done;
  }
  result = service->serializer.vtable.store_transactions_serialize_request(req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(result));
    goto done;
  }

done:
  char_buffer_free(req_buff);
  char_buffer_free(res_buff);
  return result;
}
