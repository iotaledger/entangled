// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
#define CCLIENT_SERIALIZATION_JSON_SERIALIZER_H

#include <stdlib.h>
#include "serializer/serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

void json_find_transactions_serialize_request(
    const serializer* const, const find_transactions_req_t* const obj,
    char* out);

void json_find_transactions_deserialize_response(const serializer* const,
                                                 const char* const obj,
                                                 find_transactions_res_t* out);

size_t json_find_transactions_serialize_request_get_size(
    const serializer* const, const find_transactions_req_t* const toSerialize);

size_t json_find_transactions_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

// get_balances_response
void json_get_balances_serialize_request(const serializer* const,
                                         const get_balances_req_t* const obj,
                                         char* out);

void json_get_balances_deserialize_response(const serializer* const,
                                            const char* const obj,
                                            get_balances_res_t* out);

size_t json_get_balances_serialize_request_get_size_(
    const serializer* const, const get_balances_req_t* const toSerialize);

size_t json_get_balances_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

// get_inclusion_state_response
void json_get_inclusion_state_serialize_request(
    const serializer* const, const get_inclusion_state_req_t* const obj,
    char* out);

void json_get_inclusion_state_deserialize_response(
    const serializer* const, const char* const obj,
    get_inclusion_state_res_t* out);

size_t json_get_inclusion_state_serialize_request_get_size(
    const serializer* const,
    const get_inclusion_state_req_t* const toSerialize);

size_t json_get_inclusion_state_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

// get_neighbors_response

void json_get_neighbors_deserialize_response(const serializer* const,
                                             const char* const obj,
                                             get_neighbors_res_t* out);

size_t json_get_neighbors_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

// get_node_info_response

void json_get_node_info_deserialize_response(const serializer* const,
                                             const char* const obj,
                                             get_node_info_res_t* out);

size_t json_get_node_info_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

// get_tips_response

void json_get_tips_deserialize_response(const serializer* const,
                                        const char* const obj,
                                        get_tips_res_t* out);

size_t json_get_tips_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

// get_transactions_to_approve_response
void json_get_transactions_to_approve_serialize_request(const serializer* const,
                                                        int depth, char* out);

void json_get_transactions_to_approve_deserialize_response(
    const serializer* const, const char* const obj,
    get_transactions_to_approve_res_t* out);

size_t json_get_transactions_to_approve_serialize_request_get_size(
    const serializer* const);

size_t json_get_transactions_to_approve_deserialize_response_get_size(
    const serializer* const, const char* const toDesirialize);

static serializer_vtable json_vtable = {
    .find_transactions_serialize_request =
        json_find_transactions_serialize_request,
    .find_transactions_deserialize_response =
        json_find_transactions_deserialize_response,
    .find_transactions_serialize_request_get_size =
        json_find_transactions_serialize_request_get_size,
    .find_transactions_deserialize_response_get_size =
        json_find_transactions_deserialize_response_get_size,
    .get_balances_serialize_request = json_get_balances_serialize_request,
    .get_balances_deserialize_response = json_get_balances_deserialize_response,
    .get_balances_serialize_request_get_size_ =
        json_get_balances_serialize_request_get_size_,
    .get_balances_deserialize_response_get_size =
        json_get_balances_deserialize_response_get_size,
    .get_inclusion_state_serialize_request =
        json_get_inclusion_state_serialize_request,
    .get_inclusion_state_deserialize_response =
        json_get_inclusion_state_deserialize_response,
    .get_inclusion_state_serialize_request_get_size =
        json_get_inclusion_state_serialize_request_get_size,
    .get_inclusion_state_deserialize_response_get_size =
        json_get_inclusion_state_deserialize_response_get_size,
    .get_neighbors_deserialize_response =
        json_get_neighbors_deserialize_response,
    .get_neighbors_deserialize_response_get_size =
        json_get_neighbors_deserialize_response_get_size,
    .get_node_info_deserialize_response =
        json_get_node_info_deserialize_response,
    .get_node_info_deserialize_response_get_size =
        json_get_node_info_deserialize_response_get_size,
    .get_tips_deserialize_response = json_get_tips_deserialize_response,
    .get_tips_deserialize_response_get_size =
        json_get_tips_deserialize_response_get_size,
    .get_transactions_to_approve_serialize_request =
        json_get_transactions_to_approve_serialize_request,
    .get_transactions_to_approve_deserialize_response =
        json_get_transactions_to_approve_deserialize_response,
    .get_transactions_to_approve_deserialize_response_get_size =
        json_get_transactions_to_approve_deserialize_response_get_size,
};

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
