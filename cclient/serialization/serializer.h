// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_SERIALIZATION_SERIALIZER_H
#define CCLIENT_SERIALIZATION_SERIALIZER_H

#include <stdlib.h>
#include "request/requests.h"
#include "response/responses.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct serializer_base serializer;

typedef struct {
  // find_transactions_request
  void (*serialize_find_transactions_request)(
      const serializer* const, const find_transaction_req_t* const obj,
      char* out);

  void (*deserialize_find_transactions_request)(const serializer* const,
                                                const char* const obj,
                                                find_transaction_req_t* out);

  size_t (*get_size_serialize_find_transactions_request)(
      const serializer* const, const find_transaction_req_t* const toSerialize);

  size_t (*get_size_deserialize_find_transactions_request)(
      const serializer* const, const char* const toDesirialize);

  // get_balances_response
  void (*serialize_get_balances_response)(const serializer* const,
                                          const get_balances_res_t* const obj,
                                          char* out);

  void (*deserialize_get_balances_request_response)(const serializer* const,
                                                    const char* const obj,
                                                    get_balances_res_t* out);

  size_t (*get_size_serialize_get_balances_response)(
      const serializer* const, const get_balances_res_t* const toSerialize);

  size_t (*get_size_deserialize_get_balances_response)(
      const serializer* const, const char* const toDesirialize);

  // get_inclusion_state_response
  void (*serialize_get_inclusion_state_response)(
      const serializer* const, const get_inclusion_state_res_t* const obj,
      char* out);

  void (*deserialize_get_inclusion_state_response)(
      const serializer* const, const char* const obj,
      get_inclusion_state_res_t* out);

  size_t (*get_size_serialize_get_inclusion_state_response)(
      const serializer* const,
      const get_inclusion_state_res_t* const toSerialize);

  size_t (*get_size_deserialize_get_inclusion_state_response)(
      const serializer* const, const char* const toDesirialize);

  // get_neighbors_response
  void (*serialize_get_neighbors_response)(const serializer* const,
                                           const get_neighbors_res_t* const obj,
                                           char* out);

  void (*deserialize_get_neighbors_response)(const serializer* const,
                                             const char* const obj,
                                             get_neighbors_res_t* out);

  size_t (*get_size_serialize_get_neighbors_response)(
      const serializer* const, const get_neighbors_res_t* const toSerialize);

  size_t (*get_size_deserialize_get_neighbors_response)(
      const serializer* const, const char* const toDesirialize);

  // get_node_info_response
  void (*serialize_get_node_info_response)(const serializer* const,
                                           const get_node_info_res_t* const obj,
                                           char* out);

  void (*deserialize_get_node_info_response)(const serializer* const,
                                             const char* const obj,
                                             get_node_info_res_t* out);

  size_t (*get_size_serialize_get_node_info_response)(
      const serializer* const, const get_node_info_res_t* const toSerialize);

  size_t (*get_size_deserialize_get_node_info_response)(
      const serializer* const, const char* const toDesirialize);

  // get_tips_response
  void (*serialize_get_tips_response)(const serializer* const,
                                      const get_tips_res_t* const obj,
                                      char* out);

  void (*deserialize_get_tips_response)(const serializer* const,
                                        const char* const obj,
                                        get_tips_res_t* out);

  size_t (*get_size_serialize_get_tips_response)(
      const serializer* const, const get_tips_res_t* const toSerialize);

  size_t (*get_size_deserialize_get_tips_response)(
      const serializer* const, const char* const toDesirialize);

  // get_transactions_to_approve_response
  void (*serialize_get_transactions_to_approve_response)(
      const serializer* const,
      const get_transactions_to_approve_res_t* const obj, char* out);

  void (*deserialize_get_transactions_to_approve)(
      const serializer* const, const char* const obj,
      get_transactions_to_approve_res_t* out);

  size_t (*get_size_serialize_get_transactions_to_approve)(
      const serializer* const,
      const get_transactions_to_approve_res_t* const toSerialize);

  size_t (*get_size_deserialize_get_transactions_to_approve)(
      const serializer* const, const char* const toDesirialize);

} serializer_vtable;

typedef struct serializer_base {
  serializer_vtable* vtable;

} serializer_base;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_SERIALIZER_H
