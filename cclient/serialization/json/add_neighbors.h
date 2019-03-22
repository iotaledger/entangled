/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS_H
#define CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/add_neighbors.h"
#include "cclient/response/add_neighbors.h"
#include "cclient/serialization/serializer.h"

retcode_t json_add_neighbors_serialize_request(const serializer_t* const s, const add_neighbors_req_t* const obj,
                                               char_buffer_t* out);

retcode_t json_add_neighbors_serialize_response(const serializer_t* const s, const add_neighbors_res_t* const obj,
                                                char_buffer_t* out);

retcode_t json_add_neighbors_deserialize_request(const serializer_t* const s, const char* const obj,
                                                 add_neighbors_req_t* out);

retcode_t json_add_neighbors_deserialize_response(const serializer_t* const s, const char* const obj,
                                                  add_neighbors_res_t* out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS
