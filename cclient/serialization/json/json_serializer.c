#include "json_serializer.h"

void init_json_serializer(serializer_base* serializer) {
  serializer->vtable = &json_vtable;
}

void json_serialize_find_transactions_request(
    const serializer* const serializer, const find_transaction_req_t* const obj,
    char* out) {}
void json_deserialize_find_transactions_request(
    const serializer* const serializer, const char* const obj,
    find_transaction_req_t* out) {}
size_t json_get_size_serialize_find_transactions_request(
    const serializer* const serializer,
    const find_transaction_req_t* const toSerialize) {}
size_t json_get_size_deserialize_find_transactions_request(
    const serializer* const serializer, const char* const toDesirialize) {}

void json_serialize_get_balances_response(const serializer* const serializer,
                                          const get_balances_res_t* const obj,
                                          char* out) {}

void json_deserialize_get_balances_request_response(
    const serializer* const serializer, const char* const obj,
    get_balances_res_t* out) {}

size_t json_get_size_serialize_get_balances_response(
    const serializer* const serializer,
    const get_balances_res_t* const toSerialize) {
  return 0;
}

size_t json_get_size_deserialize_get_balances_response(
    const serializer* const serializer, const char* const toDesirialize) {
  return 0;
}

// get_inclusion_state_response
void json_serialize_get_inclusion_state_response(
    const serializer* const serializer,
    const get_inclusion_state_res_t* const obj, char* out) {}

void json_deserialize_get_inclusion_state_response(
    const serializer* const serializer, const char* const obj,
    get_inclusion_state_res_t* out) {}

size_t json_get_size_serialize_get_inclusion_state_response(
    const serializer* const serializer,
    const get_inclusion_state_res_t* const toSerialize) {
  return 0;
}

size_t json_get_size_deserialize_get_inclusion_state_response(
    const serializer* const serializer, const char* const toDesirialize) {
  return 0;
}

// get_neighbors_response
void json_serialize_get_neighbors_response(const serializer* const serializer,
                                           const get_neighbors_res_t* const obj,
                                           char* out) {}

void json_deserialize_get_neighbors_response(const serializer* const serializer,
                                             const char* const obj,
                                             get_neighbors_res_t* out) {}

size_t json_get_size_serialize_get_neighbors_response(
    const serializer* const serializer,
    const get_neighbors_res_t* const toSerialize) {
  return 0;
}

size_t json_get_size_deserialize_get_neighbors_response(
    const serializer* const serializer, const char* const toDesirialize) {
  return 0;
}

// get_node_info_response
void json_serialize_get_node_info_response(const serializer* const serializer,
                                           const get_node_info_res_t* const obj,
                                           char* out) {}

void json_deserialize_get_node_info_response(const serializer* const serializer,
                                             const char* const obj,
                                             get_node_info_res_t* out) {}

size_t json_get_size_serialize_get_node_info_response(
    const serializer* const serializer,
    const get_node_info_res_t* const toSerialize) {
  return 0;
}

size_t json_get_size_deserialize_get_node_info_response(
    const serializer* const serializer, const char* const toDesirialize) {
  return 0;
}

// get_tips_response
void json_serialize_get_tips_response(const serializer* const serializer,
                                      const get_tips_res_t* const obj,
                                      char* out) {}

void json_deserialize_get_tips_response(const serializer* const serializer,
                                        const char* const obj,
                                        get_tips_res_t* out) {}

size_t json_get_size_serialize_get_tips_response(
    const serializer* const serializer,
    const get_tips_res_t* const toSerialize) {
  return 0;
}

size_t json_get_size_deserialize_get_tips_response(
    const serializer* const serializer, const char* const toDesirialize) {
  return 0;
}

// get_transactions_to_approve_response
void json_serialize_get_transactions_to_approve_response(
    const serializer* const serializer,
    const get_transactions_to_approve_res_t* const obj, char* out) {}

void json_deserialize_get_transactions_to_approve(
    const serializer* const serializer, const char* const obj,
    get_transactions_to_approve_res_t* out) {}

size_t json_get_size_serialize_get_transactions_to_approve(
    const serializer* const serializer,
    const get_transactions_to_approve_res_t* const toSerialize) {
  return 0;
}

size_t json_get_size_deserialize_get_transactions_to_approve(
    const serializer* const serializer, const char* const toDesirialize) {
  return 0;
}
