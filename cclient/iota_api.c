/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "iota_api.h"
#include "cclient/http/http.h"
#include "cclient/service.h"

#define CCLIENT_CORE_LOGGER_ID "cclient_core_api"

void iota_client_core_init(iota_client_service_t* serv) {
  logger_helper_init(CCLIENT_CORE_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d] enable logger %s.\n", __func__,
           __LINE__, CCLIENT_CORE_LOGGER_ID);
  iota_client_service_init(serv);
}

void iota_client_core_destroy() {
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d] destroy logger %s.\n", __func__,
           __LINE__, CCLIENT_CORE_LOGGER_ID);
  logger_helper_destroy(CCLIENT_CORE_LOGGER_ID);
  iota_client_service_destroy();
}

retcode_t iota_api_get_node_info(const iota_client_service_t* const service,
                                 get_node_info_res_t** res) {
  retcode_t result = RC_OK;

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.get_node_info_serialize_request(
      &service->serializer, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_node_info_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_get_neighbors(const iota_client_service_t* const service,
                                 get_neighbors_res_t* const res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.get_neighbors_serialize_request(
      &service->serializer, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_neighbors_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_add_neighbors(const iota_client_service_t* const service,
                                 add_neighbors_req_t* req,
                                 add_neighbors_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.add_neighbors_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.add_neighbors_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_remove_neighbors(const iota_client_service_t* const service,
                                    remove_neighbors_req_t* req,
                                    remove_neighbors_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.remove_neighbors_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.remove_neighbors_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_get_tips(const iota_client_service_t* const service,
                            get_tips_res_t** res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.get_tips_serialize_request(
      &service->serializer, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_tips_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_find_transactions(const iota_client_service_t* const service,
                                     const find_transactions_req_t* const req,
                                     find_transactions_res_t** const res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.find_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.find_transactions_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_get_trytes(const iota_client_service_t* const service,
                              get_trytes_req_t* req, get_trytes_res_t** res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.get_trytes_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_trytes_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_get_inclusion_states(
    const iota_client_service_t* const service, get_inclusion_state_req_t* req,
    get_inclusion_state_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.get_inclusion_state_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_inclusion_state_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_get_balances(const iota_client_service_t* const service,
                                const get_balances_req_t* const req,
                                get_balances_res_t** res) {
  retcode_t result = RC_OK;

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.get_balances_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_balances_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }

  return result;
}

retcode_t iota_api_get_transactions_to_approve(
    const iota_client_service_t* const service,
    get_transactions_to_approve_req_t* req,
    get_transactions_to_approve_res_t** res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result =
      service->serializer.vtable.get_transactions_to_approve_serialize_request(
          &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable
               .get_transactions_to_approve_deserialize_response(
                   &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_attach_to_tangle(const iota_client_service_t* const service,
                                    attach_to_tangle_req_t* req,
                                    attach_to_tangle_res_t** res) {
  retcode_t result = RC_OK;

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.attach_to_tangle_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.attach_to_tangle_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_api_interrupt_attaching_to_tangle(
    const iota_client_service_t* const service) {
  retcode_t result = RC_OK;
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  // TODO
  return result;
}

retcode_t iota_api_broadcast_transactions(
    const iota_client_service_t* const service,
    broadcast_transactions_req_t* req) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.broadcast_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }

  return result;
}

retcode_t iota_api_store_transactions(
    const iota_client_service_t* const service, store_transactions_req_t* req) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.store_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }

  return result;
}

retcode_t iota_api_check_consistency(const iota_client_service_t* const service,
                                     check_consistency_req_t* req,
                                     check_consistency_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(CCLIENT_CORE_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.check_consistency_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(CCLIENT_CORE_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.check_consistency_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}
