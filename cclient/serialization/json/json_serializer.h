// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
#define CCLIENT_SERIALIZATION_JSON_SERIALIZER_H

#include <stdlib.h>
#include "serializer/serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_json_serializer(serializer_base* serializer);

void json_serialize_find_transactions_request(
    const serializer* const, const find_transaction_req_t* const obj,
    char* out);

void json_deserialize_find_transactions_request(const serializer* const,
                                                const char* const obj,
                                                find_transaction_req_t* out);

size_t json_get_size_serialize_find_transactions_request(
    const serializer* const, const find_transaction_req_t* const toSerialize);

size_t json_get_size_deserialize_find_transactions_request(
    const serializer* const, const char* const toDesirialize);

void json_serialize_get_balances_response(const serializer* const,
                                          const get_balances_res_t* const obj,
                                          char* out);

void json_deserialize_get_balances_request_response(const serializer* const,
                                                    const char* const obj,
                                                    get_balances_res_t* out);

size_t json_get_size_serialize_get_balances_response(
    const serializer* const, const get_balances_res_t* const toSerialize);

size_t json_get_size_deserialize_get_balances_response(
    const serializer* const, const char* const toDesirialize);

// get_inclusion_state_response
void json_serialize_get_inclusion_state_response(
    const serializer* const, const get_inclusion_state_res_t* const obj,
    char* out);

void json_deserialize_get_inclusion_state_response(
    const serializer* const, const char* const obj,
    get_inclusion_state_res_t* out);

size_t json_get_size_serialize_get_inclusion_state_response(
    const serializer* const,
    const get_inclusion_state_res_t* const toSerialize);

size_t json_get_size_deserialize_get_inclusion_state_response(
    const serializer* const, const char* const toDesirialize);

// get_neighbors_response
void json_serialize_get_neighbors_response(const serializer* const,
                                           const get_neighbors_res_t* const obj,
                                           char* out);

void json_deserialize_get_neighbors_response(const serializer* const,
                                             const char* const obj,
                                             get_neighbors_res_t* out);

size_t json_get_size_serialize_get_neighbors_response(
    const serializer* const, const get_neighbors_res_t* const toSerialize);

size_t json_get_size_deserialize_get_neighbors_response(
    const serializer* const, const char* const toDesirialize);

// get_node_info_response
void json_serialize_get_node_info_response(const serializer* const,
                                           const get_node_info_res_t* const obj,
                                           char* out);

void json_deserialize_get_node_info_response(const serializer* const,
                                             const char* const obj,
                                             get_node_info_res_t* out);

size_t json_get_size_serialize_get_node_info_response(
    const serializer* const, const get_node_info_res_t* const toSerialize);

size_t json_get_size_deserialize_get_node_info_response(
    const serializer* const, const char* const toDesirialize);

// get_tips_response
void json_serialize_get_tips_response(const serializer* const,
                                      const get_tips_res_t* const obj,
                                      char* out);

void json_deserialize_get_tips_response(const serializer* const,
                                        const char* const obj,
                                        get_tips_res_t* out);

size_t json_get_size_serialize_get_tips_response(
    const serializer* const, const get_tips_res_t* const toSerialize);

size_t json_get_size_deserialize_get_tips_response(
    const serializer* const, const char* const toDesirialize);

// get_transactions_to_approve_response
void json_serialize_get_transactions_to_approve_response(
    const serializer* const, const get_transactions_to_approve_res_t* const obj,
    char* out);

void json_deserialize_get_transactions_to_approve(
    const serializer* const, const char* const obj,
    get_transactions_to_approve_res_t* out);

size_t json_get_size_serialize_get_transactions_to_approve(
    const serializer* const,
    const get_transactions_to_approve_res_t* const toSerialize);

size_t json_get_size_deserialize_get_transactions_to_approve(
    const serializer* const, const char* const toDesirialize);

static serializer_vtable json_vtable = {
    .serialize_find_transactions_request =
        json_serialize_find_transactions_request,
    .deserialize_find_transactions_request =
        json_deserialize_find_transactions_request,
    .get_size_serialize_find_transactions_request =
        json_get_size_serialize_find_transactions_request,
    .get_size_deserialize_find_transactions_request =
        json_get_size_deserialize_find_transactions_request,
    .serialize_get_balances_response = json_serialize_get_balances_response,
    .deserialize_get_balances_request_response =
        json_deserialize_get_balances_request_response,
    .get_size_serialize_get_balances_response =
        json_get_size_serialize_get_balances_response,
    .get_size_deserialize_get_balances_response =
        json_get_size_deserialize_get_balances_response,
    .serialize_get_inclusion_state_response =
        json_serialize_get_inclusion_state_response,
    .deserialize_get_inclusion_state_response =
        json_deserialize_get_inclusion_state_response,
    .get_size_serialize_get_inclusion_state_response =
        json_get_size_serialize_get_inclusion_state_response,
    .get_size_deserialize_get_inclusion_state_response =
        json_get_size_deserialize_get_inclusion_state_response,
    .serialize_get_neighbors_response = json_serialize_get_neighbors_response,
    .deserialize_get_neighbors_response =
        json_deserialize_get_neighbors_response,
    .get_size_serialize_get_neighbors_response =
        json_get_size_serialize_get_neighbors_response,
    .get_size_deserialize_get_neighbors_response =
        json_get_size_deserialize_get_neighbors_response,
    .serialize_get_node_info_response = json_serialize_get_node_info_response,
    .deserialize_get_node_info_response =
        json_deserialize_get_node_info_response,
    .get_size_serialize_get_node_info_response =
        json_get_size_serialize_get_node_info_response,
    .get_size_deserialize_get_node_info_response =
        json_get_size_deserialize_get_node_info_response,
    .serialize_get_tips_response = json_serialize_get_tips_response,
    .deserialize_get_tips_response = json_deserialize_get_tips_response,
    .get_size_serialize_get_tips_response =
        json_get_size_serialize_get_tips_response,
    .get_size_deserialize_get_tips_response =
        json_get_size_deserialize_get_tips_response,
    .serialize_get_transactions_to_approve_response =
        json_serialize_get_transactions_to_approve_response,
    .deserialize_get_transactions_to_approve =
        json_deserialize_get_transactions_to_approve,
    .get_size_serialize_get_transactions_to_approve =
        json_get_size_serialize_get_transactions_to_approve,
    .get_size_deserialize_get_transactions_to_approve =
        json_get_size_deserialize_get_transactions_to_approve,
};

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
