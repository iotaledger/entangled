/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_SERIALIZER_H
#define CCLIENT_SERIALIZATION_SERIALIZER_H

#include <stdlib.h>
#include "cclient/request/requests.h"
#include "cclient/response/responses.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum serializer_type_e {
  SR_JSON = 0,
  SR_UNIMPLEMENTED,
} serializer_type_t;

typedef struct serializer_base serializer_t;

typedef struct {
  retcode_t (*add_neighbors_serialize_request)(serializer_t const* const s, add_neighbors_req_t const* const obj,
                                               char_buffer_t* out);
  retcode_t (*add_neighbors_serialize_response)(serializer_t const* const s, add_neighbors_res_t const* const obj,
                                                char_buffer_t* out);
  retcode_t (*add_neighbors_deserialize_request)(serializer_t const* const s, char const* const obj,
                                                 add_neighbors_req_t* out);
  retcode_t (*add_neighbors_deserialize_response)(serializer_t const* const s, char const* const obj,
                                                  add_neighbors_res_t* out);

  retcode_t (*find_transactions_serialize_request)(serializer_t const* const s,
                                                   find_transactions_req_t const* const obj, char_buffer_t* out);

  retcode_t (*find_transactions_deserialize_response)(serializer_t const* const s, char const* const obj,
                                                      find_transactions_res_t* out);

  retcode_t (*get_balances_serialize_request)(serializer_t const* const s, get_balances_req_t const* const obj,
                                              char_buffer_t* out);

  retcode_t (*get_balances_deserialize_request)(serializer_t const* const s, char const* const obj,
                                                get_balances_req_t* const out);

  retcode_t (*get_balances_serialize_response)(serializer_t const* const s, get_balances_res_t const* const obj,
                                               char_buffer_t* out);

  retcode_t (*get_balances_deserialize_response)(serializer_t const* const s, char const* const obj,
                                                 get_balances_res_t* const out);

  retcode_t (*get_inclusion_states_serialize_request)(serializer_t const* const, get_inclusion_states_req_t* const obj,
                                                      char_buffer_t* out);

  retcode_t (*get_inclusion_states_deserialize_response)(serializer_t const* const, char const* const obj,
                                                         get_inclusion_states_res_t* out);

  retcode_t (*get_neighbors_serialize_request)(serializer_t const* const s, char_buffer_t* out);

  retcode_t (*get_neighbors_serialize_response)(serializer_t const* const s, get_neighbors_res_t const* const obj,
                                                char_buffer_t* out);

  retcode_t (*get_neighbors_deserialize_response)(serializer_t const* const, char const* const obj,
                                                  get_neighbors_res_t* out);

  retcode_t (*get_node_info_serialize_request)(serializer_t const* const, char_buffer_t* out);

  retcode_t (*get_node_info_deserialize_response)(serializer_t const* const, const char* const obj,
                                                  get_node_info_res_t* out);

  retcode_t (*get_node_info_serialize_response)(const serializer_t* const, const get_node_info_res_t* const obj,
                                                char_buffer_t* out);

  retcode_t (*get_tips_serialize_request)(serializer_t const* const, char_buffer_t* out);

  retcode_t (*get_tips_serialize_response)(serializer_t const* const s, get_tips_res_t const* const res,
                                           char_buffer_t* out);
  retcode_t (*get_tips_deserialize_response)(serializer_t const* const, const char* const obj, get_tips_res_t* res);

  retcode_t (*get_transactions_to_approve_serialize_request)(serializer_t const* const,
                                                             get_transactions_to_approve_req_t const* const obj,
                                                             char_buffer_t* out);
  retcode_t (*get_transactions_to_approve_deserialize_response)(serializer_t const* const, char const* const obj,
                                                                get_transactions_to_approve_res_t* out);

  retcode_t (*remove_neighbors_serialize_request)(serializer_t const* const s, remove_neighbors_req_t const* const obj,
                                                  char_buffer_t* out);

  retcode_t (*remove_neighbors_deserialize_request)(serializer_t const* const s, char const* const obj,
                                                    remove_neighbors_req_t* out);

  retcode_t (*remove_neighbors_serialize_response)(serializer_t const* const s, remove_neighbors_res_t const* const obj,
                                                   char_buffer_t* out);

  retcode_t (*remove_neighbors_deserialize_response)(serializer_t const* const s, char const* const obj,
                                                     remove_neighbors_res_t* out);

  retcode_t (*get_trytes_serialize_request)(serializer_t const* const s, get_trytes_req_t const* const req,
                                            char_buffer_t* out);
  retcode_t (*get_trytes_deserialize_response)(serializer_t const* const s, char const* const obj,
                                               get_trytes_res_t* const res);

  retcode_t (*attach_to_tangle_serialize_request)(serializer_t const* const s, attach_to_tangle_req_t const* const obj,
                                                  char_buffer_t* out);
  retcode_t (*attach_to_tangle_serialize_response)(serializer_t const* const s, attach_to_tangle_res_t const* const obj,
                                                   char_buffer_t* out);
  retcode_t (*attach_to_tangle_deserialize_request)(serializer_t const* const s, char const* const obj,
                                                    attach_to_tangle_req_t* out);
  retcode_t (*attach_to_tangle_deserialize_response)(serializer_t const* const s, char const* const obj,
                                                     attach_to_tangle_res_t* out);

  retcode_t (*broadcast_transactions_serialize_request)(serializer_t const* const s,
                                                        broadcast_transactions_req_t* const obj, char_buffer_t* out);
  retcode_t (*broadcast_transactions_deserialize_request)(serializer_t const* const s, char const* const obj,
                                                          broadcast_transactions_req_t* const out);

  retcode_t (*store_transactions_serialize_request)(serializer_t const* const s,
                                                    store_transactions_req_t const* const obj, char_buffer_t* out);

  retcode_t (*check_consistency_serialize_request)(serializer_t const* const s, check_consistency_req_t* const obj,
                                                   char_buffer_t* out);
  retcode_t (*check_consistency_serialize_response)(serializer_t const* const s, check_consistency_res_t* const obj,
                                                    char_buffer_t* out);
  retcode_t (*check_consistency_deserialize_request)(serializer_t const* const s, char const* const obj,
                                                     check_consistency_req_t* out);
  retcode_t (*check_consistency_deserialize_response)(serializer_t const* const s, char const* const obj,
                                                      check_consistency_res_t* out);
} serializer_vtable;

typedef struct serializer_base {
  serializer_vtable vtable;
} serializer_base;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_SERIALIZER_H
