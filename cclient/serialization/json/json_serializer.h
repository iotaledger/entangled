/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
#define CCLIENT_SERIALIZATION_JSON_SERIALIZER_H

#include <stdlib.h>

#include "cclient/serialization/serializer.h"
#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

void logger_init_json_serializer();
void logger_destroy_json_serializer();

void init_json_serializer(serializer_t* serializer);

retcode_t json_find_transactions_serialize_request(
    serializer_t const* const s, find_transactions_req_t const* const obj,
    char_buffer_t* out);

retcode_t json_find_transactions_deserialize_response(
    serializer_t const* const, char const* const obj,
    find_transactions_res_t* out);

// get_balances_response
retcode_t json_get_balances_serialize_request(
    serializer_t const* const, get_balances_req_t const* const obj,
    char_buffer_t* out);

retcode_t json_get_balances_deserialize_response(serializer_t const* const,
                                                 char const* const obj,
                                                 get_balances_res_t* out);

// get_inclusion_state_response
retcode_t json_get_inclusion_state_serialize_request(
    const serializer_t* const, get_inclusion_state_req_t* const obj,
    char_buffer_t* out);

retcode_t json_get_inclusion_state_deserialize_response(
    const serializer_t* const, const char* const obj,
    get_inclusion_state_res_t* out);

retcode_t json_get_neighbors_serialize_request(const serializer_t* const s,
                                               char_buffer_t* out);

// get_neighbors_response
retcode_t json_get_neighbors_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  get_neighbors_res_t* out);

// get_node_info
retcode_t json_get_node_info_serialize_request(const serializer_t* const s,
                                               char_buffer_t* out);

retcode_t json_get_node_info_deserialize_response(const serializer_t* const,
                                                  const char* const obj,
                                                  get_node_info_res_t** out);

// get_tips_response
retcode_t json_get_tips_serialize_request(const serializer_t* const s,
                                          char_buffer_t* out);
retcode_t json_get_tips_deserialize_response(const serializer_t* const s,
                                             const char* const obj,
                                             get_tips_res_t* res);

// get_transactions_to_approve_response
retcode_t json_get_transactions_to_approve_serialize_request(
    const serializer_t* const,
    const get_transactions_to_approve_req_t* const obj, char_buffer_t* out);

retcode_t json_get_transactions_to_approve_deserialize_response(
    const serializer_t* const, const char* const obj,
    get_transactions_to_approve_res_t** out);

// addNeighbors
retcode_t json_add_neighbors_serialize_request(
    const serializer_t* const s, const add_neighbors_req_t* const obj,
    char_buffer_t* out);

retcode_t json_add_neighbors_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  add_neighbors_res_t* out);
// removeNeighbors
retcode_t json_remove_neighbors_serialize_request(
    const serializer_t* const s, const remove_neighbors_req_t* const obj,
    char_buffer_t* out);

retcode_t json_remove_neighbors_deserialize_response(
    const serializer_t* const s, const char* const obj,
    remove_neighbors_res_t* out);

retcode_t json_get_trytes_serialize_request(const serializer_t* const s,
                                            get_trytes_req_t const* const req,
                                            char_buffer_t* out);
retcode_t json_get_trytes_deserialize_response(const serializer_t* const s,
                                               const char* const obj,
                                               get_trytes_res_t* const res);

retcode_t json_attach_to_tangle_serialize_request(
    const serializer_t* const s, const attach_to_tangle_req_t* const obj,
    char_buffer_t* out);
retcode_t json_attach_to_tangle_deserialize_response(
    const serializer_t* const s, const char* const obj,
    attach_to_tangle_res_t** out);

// broadcast_transactions_request
retcode_t json_broadcast_transactions_serialize_request(
    const serializer_t* const s, broadcast_transactions_req_t* const obj,
    char_buffer_t* out);

// store_transactions request
retcode_t json_store_transactions_serialize_request(
    const serializer_t* const s, store_transactions_req_t* const obj,
    char_buffer_t* out);

retcode_t json_check_consistency_serialize_request(
    const serializer_t* const s, check_consistency_req_t* const obj,
    char_buffer_t* out);
retcode_t json_check_consistency_deserialize_response(
    const serializer_t* const s, const char* const obj,
    check_consistency_res_t* out);

static serializer_vtable json_vtable = {
    .find_transactions_serialize_request =
        json_find_transactions_serialize_request,
    .find_transactions_deserialize_response =
        json_find_transactions_deserialize_response,
    .get_balances_serialize_request = json_get_balances_serialize_request,
    .get_balances_deserialize_response = json_get_balances_deserialize_response,
    .get_inclusion_state_serialize_request =
        json_get_inclusion_state_serialize_request,
    .get_inclusion_state_deserialize_response =
        json_get_inclusion_state_deserialize_response,
    .get_neighbors_serialize_request = json_get_neighbors_serialize_request,
    .get_neighbors_deserialize_response =
        json_get_neighbors_deserialize_response,
    .get_node_info_serialize_request = json_get_node_info_serialize_request,
    .get_node_info_deserialize_response =
        json_get_node_info_deserialize_response,
    .get_tips_serialize_request = json_get_tips_serialize_request,
    .get_tips_deserialize_response = json_get_tips_deserialize_response,
    .get_transactions_to_approve_serialize_request =
        json_get_transactions_to_approve_serialize_request,
    .get_transactions_to_approve_deserialize_response =
        json_get_transactions_to_approve_deserialize_response,
    .add_neighbors_serialize_request = json_add_neighbors_serialize_request,
    .add_neighbors_deserialize_response =
        json_add_neighbors_deserialize_response,
    .remove_neighbors_serialize_request =
        json_remove_neighbors_serialize_request,
    .remove_neighbors_deserialize_response =
        json_remove_neighbors_deserialize_response,
    .get_trytes_serialize_request = json_get_trytes_serialize_request,
    .get_trytes_deserialize_response = json_get_trytes_deserialize_response,
    .attach_to_tangle_serialize_request =
        json_attach_to_tangle_serialize_request,
    .attach_to_tangle_deserialize_response =
        json_attach_to_tangle_deserialize_response,
    .broadcast_transactions_serialize_request =
        json_broadcast_transactions_serialize_request,
    .store_transactions_serialize_request =
        json_store_transactions_serialize_request,
    .check_consistency_serialize_request =
        json_check_consistency_serialize_request,
    .check_consistency_deserialize_response =
        json_check_consistency_deserialize_response,
};

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
