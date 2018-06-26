#include "json_serializer.h"
#include "cJSON.h"

void init_json_serializer(serializer_base* serializer) {
  serializer->vtable = json_vtable;
}

void json_find_transactions_serialize_request(
    const serializer* const s, const find_transactions_req_t* const obj,
    char* out) {}

void json_find_transactions_deserialize_response(const serializer* const s,
                                                 const char* const obj,
                                                 find_transactions_res_t* out) {
}

size_t json_find_transactions_serialize_request_get_size(
    const serializer* const s,
    const find_transactions_req_t* const toSerialize) {
  return 0;
}

size_t json_find_transactions_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}

// get_balances_response
void json_get_balances_serialize_request(const serializer* const s,
                                         const get_balances_req_t* const obj,
                                         char* out) {}

void json_get_balances_deserialize_response(const serializer* const s,
                                            const char* const obj,
                                            get_balances_res_t* out) {}

size_t json_get_balances_serialize_request_get_size_(
    const serializer* const s, const get_balances_req_t* const toSerialize) {
  return 0;
}

size_t json_get_balances_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}

// get_inclusion_state_response
void json_get_inclusion_state_serialize_request(
    const serializer* const s, const get_inclusion_state_req_t* const obj,
    char* out) {}

void json_get_inclusion_state_deserialize_response(
    const serializer* const s, const char* const obj,
    get_inclusion_state_res_t* out) {}

size_t json_get_inclusion_state_serialize_request_get_size(
    const serializer* const s,
    const get_inclusion_state_req_t* const toSerialize) {
  return 0;
}

size_t json_get_inclusion_state_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}

// get_neighbors_response

void json_get_neighbors_deserialize_response(const serializer* const s,
                                             const char* const obj,
                                             get_neighbors_res_t* out) {}

size_t json_get_neighbors_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}

// get_node_info_response

void json_get_node_info_deserialize_response(const serializer* const s,
                                             const char* const obj,
                                             get_node_info_res_t* out) {}

size_t json_get_node_info_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}

// get_tips_response

void json_get_tips_deserialize_response(const serializer* const s,
                                        const char* const obj,
                                        get_tips_res_t* out) {}

size_t json_get_tips_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}

// get_transactions_to_approve_response
void json_get_transactions_to_approve_serialize_request(
    const serializer* const s, int depth, char* out) {}

void json_get_transactions_to_approve_deserialize_response(
    const serializer* const s, const char* const obj,
    get_transactions_to_approve_res_t* out) {}

size_t json_get_transactions_to_approve_serialize_request_get_size(
    const serializer* const s) {
  return 0;
}

size_t json_get_transactions_to_approve_deserialize_response_get_size(
    const serializer* const s, const char* const toDesirialize) {
  return 0;
}
