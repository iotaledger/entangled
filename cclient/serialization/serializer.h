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

typedef struct serializer_base serializer_t;

typedef struct {
  // find_transactions_request
  retcode_t (*find_transactions_serialize_request)(
      const serializer_t* const s, const find_transactions_req_t* const obj,
      char_buffer_t* out);

  retcode_t (*find_transactions_deserialize_response)(
      const serializer_t* const s, const char* const obj,
      find_transactions_res_t* out);

  // get_balances_response
  void (*get_balances_serialize_request)(const serializer_t* const,
                                         const get_balances_req_t* const obj,
                                         char* out);

  void (*get_balances_deserialize_response)(const serializer_t* const,
                                            const char* const obj,
                                            get_balances_res_t* out);

  // get_inclusion_state_response
  void (*get_inclusion_state_serialize_request)(
      const serializer_t* const, const get_inclusion_state_req_t* const obj,
      char* out);

  void (*get_inclusion_state_deserialize_response)(
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
                                             get_tips_res_t* out);

  // get_transactions_to_approve_response
  void (*get_transactions_to_approve_serialize_request)(
      const serializer_t* const, int depth, char* out);

  void (*get_transactions_to_approve_deserialize_response)(
      const serializer_t* const, const char* const obj,
      get_transactions_to_approve_res_t* out);

  // addNeighbors
  retcode_t (*add_neighbors_serialize_request)(const serializer_t* const s,
                                               add_neighbors_req_t* const obj,
                                               char_buffer_t* out);
  retcode_t (*add_neighbors_deserialize_response)(const serializer_t* const s,
                                                  const char* const obj,
                                                  add_neighbors_res_t* out);
  // removeNeighbors
  retcode_t (*remove_neighbors_serialize_request)(
      const serializer_t* const s, remove_neighbors_req_t* const obj,
      char_buffer_t* out);
  retcode_t (*remove_neighbors_deserialize_response)(
      const serializer_t* const s, const char* const obj,
      remove_neighbors_res_t* out);
} serializer_vtable;

typedef struct serializer_base {
  serializer_vtable vtable;

} serializer_base;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_SERIALIZER_H
