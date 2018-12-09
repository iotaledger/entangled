/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_SERIALIZER_H
#define CCLIENT_SERIALIZATION_SERIALIZER_H

#include <stdlib.h>
#include "request/requests.h"
#include "response/responses.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum serializer_type_e {
  SR_JSON = 0,
  SR_UNIMPLEMENTED,
} serializer_type_t;

typedef struct serializer_base serializer_t;

typedef struct {
  // find_transactions_request
  retcode_t (*find_transactions_serialize_request)(
      serializer_t const* const s, find_transactions_req_t const* const obj,
      char_buffer_t* out);

  retcode_t (*find_transactions_deserialize_response)(
      serializer_t const* const s, char const* const obj,
      find_transactions_res_t* out);

  // get_balances_response
  retcode_t (*get_balances_serialize_request)(
      const serializer_t* const, const get_balances_req_t* const obj,
      char_buffer_t* out);

  retcode_t (*get_balances_deserialize_response)(serializer_t const* const,
                                                 char const* const obj,
                                                 get_balances_res_t* const out);

  // get_inclusion_state_response
  retcode_t (*get_inclusion_state_serialize_request)(
      const serializer_t* const, get_inclusion_state_req_t* const obj,
      char_buffer_t* out);

  retcode_t (*get_inclusion_state_deserialize_response)(
      const serializer_t* const, const char* const obj,
      get_inclusion_state_res_t* out);

  // get_neighbors_request
  retcode_t (*get_neighbors_serialize_request)(const serializer_t* const s,
                                               char_buffer_t* out);

  retcode_t (*get_neighbors_deserialize_response)(const serializer_t* const,
                                                  const char* const obj,
                                                  get_neighbors_res_t* out);

  // get_node_info_request
  retcode_t (*get_node_info_serialize_request)(const serializer_t* const,
                                               char_buffer_t* out);

  retcode_t (*get_node_info_deserialize_response)(const serializer_t* const,
                                                  const char* const obj,
                                                  get_node_info_res_t* out);

  // get_tips_request
  retcode_t (*get_tips_serialize_request)(const serializer_t* const,
                                          char_buffer_t* out);
  retcode_t (*get_tips_deserialize_response)(const serializer_t* const,
                                             const char* const obj,
                                             get_tips_res_t* res);

  // get_transactions_to_approve_response
  retcode_t (*get_transactions_to_approve_serialize_request)(
      const serializer_t* const,
      const get_transactions_to_approve_req_t* const obj, char_buffer_t* out);

  retcode_t (*get_transactions_to_approve_deserialize_response)(
      const serializer_t* const, const char* const obj,
      get_transactions_to_approve_res_t* out);

  // addNeighbors
  retcode_t (*add_neighbors_serialize_request)(
      const serializer_t* const s, const add_neighbors_req_t* const obj,
      char_buffer_t* out);
  retcode_t (*add_neighbors_deserialize_response)(const serializer_t* const s,
                                                  const char* const obj,
                                                  add_neighbors_res_t* out);
  // removeNeighbors
  retcode_t (*remove_neighbors_serialize_request)(
      const serializer_t* const s, const remove_neighbors_req_t* const obj,
      char_buffer_t* out);
  retcode_t (*remove_neighbors_deserialize_response)(
      const serializer_t* const s, const char* const obj,
      remove_neighbors_res_t* out);
  retcode_t (*get_trytes_serialize_request)(const serializer_t* const s,
                                            get_trytes_req_t const* const req,
                                            char_buffer_t* out);
  retcode_t (*get_trytes_deserialize_response)(const serializer_t* const s,
                                               const char* const obj,
                                               get_trytes_res_t* const res);

  retcode_t (*attach_to_tangle_serialize_request)(
      const serializer_t* const s, const attach_to_tangle_req_t* const obj,
      char_buffer_t* out);
  retcode_t (*attach_to_tangle_deserialize_response)(
      const serializer_t* const s, const char* const obj,
      attach_to_tangle_res_t* out);

  // broadcastTransactions
  retcode_t (*broadcast_transactions_serialize_request)(
      const serializer_t* const s, broadcast_transactions_req_t* const obj,
      char_buffer_t* out);

  retcode_t (*store_transactions_serialize_request)(
      const serializer_t* const s, store_transactions_req_t* const obj,
      char_buffer_t* out);

  retcode_t (*check_consistency_serialize_request)(
      const serializer_t* const s, check_consistency_req_t* const obj,
      char_buffer_t* out);
  retcode_t (*check_consistency_deserialize_response)(
      const serializer_t* const s, const char* const obj,
      check_consistency_res_t* out);
} serializer_vtable;

typedef struct serializer_base {
  serializer_vtable vtable;
} serializer_base;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_SERIALIZER_H
