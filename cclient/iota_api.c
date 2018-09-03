/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "iota_api.h"
#include "cclient/http/http.h"
#include "cclient/service.h"

iota_api_result_t iota_api_get_node_info(
    const iota_http_service_t* const service, get_node_info_res_t* const res) {
  iota_api_result_t result = {0};

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();

  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }
  result.error = service->serializer.vtable.get_node_info_serialize_request(
      &service->serializer, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error = service->serializer.vtable.get_node_info_deserialize_response(
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

iota_api_result_t iota_api_get_neighbors(
    const iota_http_service_t* const service, get_neighbors_res_t* const res) {
  iota_api_result_t result = {0};

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }

  result.error = service->serializer.vtable.get_neighbors_serialize_request(
      &service->serializer, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error = service->serializer.vtable.get_neighbors_deserialize_response(
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

iota_api_result_t iota_api_add_neighbors(
    const iota_http_service_t* const service, add_neighbors_req_t* req,
    add_neighbors_res_t* res) {
  iota_api_result_t result = {0};

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }

  result.error = service->serializer.vtable.add_neighbors_serialize_request(
      &service->serializer, req, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error = service->serializer.vtable.add_neighbors_deserialize_response(
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

iota_api_result_t iota_api_remove_neighbors(
    const iota_http_service_t* const service, remove_neighbors_req_t* req,
    remove_neighbors_res_t* res) {
  iota_api_result_t result = {0};
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }

  result.error = service->serializer.vtable.remove_neighbors_serialize_request(
      &service->serializer, req, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error =
      service->serializer.vtable.remove_neighbors_deserialize_response(
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

iota_api_result_t iota_api_get_tips(const iota_http_service_t* const service,
                                    get_tips_res_t* const res) {
  iota_api_result_t result = {0};
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }

  result.error = service->serializer.vtable.get_tips_serialize_request(
      &service->serializer, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error = service->serializer.vtable.get_tips_deserialize_response(
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

iota_api_result_t iota_api_find_transactions(
    const iota_http_service_t* const service,
    const find_transactions_req_t* const req,
    find_transactions_res_t* const res) {
  iota_api_result_t result = {0};
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }
  result.error = service->serializer.vtable.find_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error =
      service->serializer.vtable.find_transactions_deserialize_response(
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

iota_api_result_t iota_api_get_trytes(const iota_http_service_t* const service,
                                      get_trytes_req_t* req,
                                      get_trytes_res_t* res) {
  iota_api_result_t result = {0};
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }
  result.error = service->serializer.vtable.get_trytes_serialize_request(
      &service->serializer, req, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error = service->serializer.vtable.get_trytes_deserialize_response(
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

iota_api_result_t iota_api_get_inclusion_states(
    const iota_http_service_t* const service,
    const get_inclusion_state_req_t* const req,
    get_inclusion_state_res_t* res) {
  iota_api_result_t result = {0};
  // TODO
  return result;
}

iota_api_result_t iota_api_get_balances(
    const iota_http_service_t* const service,
    const get_balances_req_t* const req, get_balances_res_t* res) {
  iota_api_result_t result = {0};

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }

  result.error = service->serializer.vtable.get_balances_serialize_request(
      &service->serializer, req, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result.error = service->serializer.vtable.get_balances_deserialize_response(
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

iota_api_result_t iota_api_get_transactions_to_approve(
    const iota_http_service_t* const service, int depth,
    get_transactions_to_approve_res_t* res) {
  iota_api_result_t result = {0};
  // TODO
  return result;
}

iota_api_result_t iota_api_attach_to_tangle(
    const iota_http_service_t* const service,
    const attach_to_tangle_req_t* const req) {
  iota_api_result_t result = {0};
  // TODO
  return result;
}

iota_api_result_t iota_api_interrupt_attaching_to_tangle(
    const iota_http_service_t* const service) {
  iota_api_result_t result = {0};
  // TODO
  return result;
}

iota_api_result_t iota_api_broadcast_transactions(
    const iota_http_service_t* const service,
    const broadcast_transactions_req_t* const req) {
  iota_api_result_t result = {0};
  // TODO
  return result;
}

iota_api_result_t iota_api_store_transactions(
    const iota_http_service_t* const service, store_transactions_req_t* req) {
  iota_api_result_t result = {0};
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  if (req_buff == NULL || res_buff == NULL) {
    result.error = RC_CCLIENT_OOM;
    goto done;
  }
  result.error =
      service->serializer.vtable.store_transactions_serialize_request(
          &service->serializer, req, req_buff);
  if (result.error != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result.error != RC_OK) {
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

iota_api_result_t iota_api_were_addresses_spent_from(
    const iota_http_service_t* const service,
    were_addresses_spent_from_req_t* req,
    were_addresses_spent_from_res_t* res) {
  iota_api_result_t result = {0};
  // TODO
  return result;
}
