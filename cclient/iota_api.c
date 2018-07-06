// Copyright 2018 IOTA Foundation

#include "iota_api.h"
#include "cclient/http/http.h"
#include "cclient/service.h"

iota_api_result_t iota_api_get_node_info(const iota_service_t* const service,
                                         get_node_info_res_t* const res) {
  iota_api_result_t result;
  iota_response_t response;
  char buffer[660];
  response.data = &buffer;
  response.length = 660;
  size_t req_size =
      service->serializer.vtable.get_node_info_serialize_request_get_size(
          &service->serializer);
  char request_data[req_size];

  service->serializer.vtable.get_node_info_serialize_request(
      &service->serializer, request_data);

  result = post(service, request_data, &response);

  // TODO - deserialize response

  return result;
}

iota_api_result_t iota_api_get_neighbors(const iota_service_t* const service,
                                         get_neighbors_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_add_neighbors(const iota_service_t* const service,
                                         const add_neighbors_req_t* const req) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_remove_neighbors(
    const iota_service_t* const service,
    const remove_neighbors_req_t* const req) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_tips(const iota_service_t* const service,
                                    get_tips_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_find_transactions(
    const iota_service_t* const service,
    const find_transactions_req_t* const req,
    find_transactions_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_trytes(const iota_service_t* const service,
                                      const get_trytes_req_t* const req,
                                      char* trytes[]) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_inclusion_states(
    const iota_service_t* const service,
    const get_inclusion_state_req_t* const req,
    get_inclusion_state_res_t* res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_balances(const iota_service_t* const service,
                                        const get_balances_req_t* const req,
                                        get_balances_res_t* res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_transactions_to_approve(
    const iota_service_t* const service, int depth,
    get_transactions_to_approve_res_t* res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_attach_to_tangle(
    const iota_service_t* const service,
    const attach_to_tangle_req_t* const req) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_interrupt_attaching_to_tangle(
    const iota_service_t* const service) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_broadcast_transactions(
    const iota_service_t* const service,
    const broadcast_transactions_req_t* const req) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_store_transactions(
    const iota_service_t* const service,
    const store_transactions_req_t* const req) {
  iota_api_result_t result;
  return result;
}