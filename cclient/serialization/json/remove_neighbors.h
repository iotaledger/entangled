/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_REMOVE_NEIGHBORS_H
#define CCLIENT_SERIALIZATION_JSON_REMOVE_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/remove_neighbors.h"
#include "cclient/response/remove_neighbors.h"
#include "cclient/serialization/serializer.h"

retcode_t json_remove_neighbors_serialize_request(const serializer_t* const s, const remove_neighbors_req_t* const obj,
                                                  char_buffer_t* out);

retcode_t json_remove_neighbors_deserialize_response(const serializer_t* const s, const char* const obj,
                                                     remove_neighbors_res_t* out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_REMOVE_NEIGHBORS_H
